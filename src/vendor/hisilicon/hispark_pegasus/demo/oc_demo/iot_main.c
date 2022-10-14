/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* we use the mqtt to connect to the IoT platform */
/*
 * STEPS:
 * 1, CONNECT TO THE IOT SERVER
 * 2, SUBSCRIBE  THE DEFAULT TOPIC
 * 3, WAIT FOR ANY MESSAGE COMES OR ANY MESSAGE TO SEND
 */

#include <securec.h>
#include <stdbool.h>
#include <string.h>
#include <hi_mem.h>
#include <hi_task.h>
#include "MQTTClient.h"
#include "cmsis_os2.h"
#include "iot_config.h"
#include "iot_errno.h"
#include "iot_log.h"
#include "iot_watchdog.h"
#include "ohos_init.h"
#include "iot_main.h"

// this is the configuration head
#define CN_IOT_SERVER "tcp://121.36.42.100:1883"

#define CONFIG_COMMAND_TIMEOUT 10000L
#define CN_KEEPALIVE_TIME      50
#define CN_CLEANSESSION        1
#define CN_HMAC_PWD_LEN        65 // SHA256 IS 32 BYTES AND END APPEND'\0'
#define CN_EVENT_TIME          "1970000100"
#define CN_CLIENTID_FMT        "%s_0_0_%s" // This is the cient ID format, deviceID_0_0_TIME
#define CN_QUEUE_WAITTIMEOUT   1000
#define CN_QUEUE_MSGNUM        16
#define CN_QUEUE_MSGSIZE       (sizeof(hi_pvoid))

#define CN_TASK_PRIOR     28
#define CN_TASK_STACKSIZE 0X2000
#define CN_TASK_NAME      "IoTMain"

typedef enum {
    EN_IOT_MSG_PUBLISH = 0,
    EN_IOT_MSG_RECV,
} EnIotMsgT;

typedef struct {
    EnIotMsgT type;
    int qos;
    char* topic;
    char* payload;
} IoTMsgT;

typedef struct {
    bool stop;
    char conLost;
    void* queueID;
    unsigned int iotTaskID;
    FnMsgCallBack msgCallBack;
    MQTTClient_deliveryToken tocken;
} IotAppCbT;
static IotAppCbT g_ioTAppCb;

static const char* g_defaultSubscribeTopic[] = {
    "$oc/devices/" CONFIG_DEVICE_ID "/sys/messages/down",
    "$oc/devices/" CONFIG_DEVICE_ID "/sys/properties/set/#",
    "$oc/devices/" CONFIG_DEVICE_ID "/sys/properties/get/#",
    "$oc/devices/" CONFIG_DEVICE_ID "/sys/shadow/get/response/#",
    "$oc/devices/" CONFIG_DEVICE_ID "/sys/events/down",
    "$oc/devices/" CONFIG_DEVICE_ID "/sys/commands/#"
};

#define CN_TOPIC_SUBSCRIBE_NUM (sizeof(g_defaultSubscribeTopic) / sizeof(const char*))

static int MsgRcvCallBack(void* context, char* topic, int topicLen, MQTTClient_message* message)
{
    (void) context;
    IoTMsgT* msg;
    char* buf;
    hi_u32 bufSize;
    int topicLenght = topicLen;

    if (topicLenght == 0) {
        topicLenght = strlen(topic);
    }
    bufSize = topicLenght + 1 + message->payloadlen + 1 + sizeof(IoTMsgT);
    buf = hi_malloc(0, bufSize);
    if (buf != NULL) {
        msg = (IoTMsgT*)buf;
        buf += sizeof(IoTMsgT);
        bufSize -= sizeof(IoTMsgT);
        msg->qos = message->qos;
        msg->type = EN_IOT_MSG_RECV;
        (void)memcpy_s(buf, bufSize, topic, topicLenght);
        buf[topicLenght] = '\0';
        msg->topic = buf;
        buf += topicLenght + 1;
        bufSize -= (topicLenght + 1);
        (void)memcpy_s(buf, bufSize, message->payload, message->payloadlen);
        buf[message->payloadlen] = '\0';
        msg->payload = buf;
        printf("RCVMSG:QOS:%d TOPIC:%s PAYLOAD:%s\r\n", msg->qos, msg->topic, msg->payload);
        if (IOT_SUCCESS != osMessageQueuePut(g_ioTAppCb.queueID, &msg, 0, CN_QUEUE_WAITTIMEOUT)) {
            IOT_LOG_ERROR("Wrie queue failed\r\n");
            hi_free(0, msg);
        }
    }

    MQTTClient_freeMessage(&message);
    MQTTClient_free(topic);
    return 1;
}

// when the connect lost and this callback will be called
static void ConnLostCallBack(void* context, char* cause)
{
    (void) context;
    printf("Connection lost:caused by:%s\r\n", cause == NULL ? "Unknown" : cause);
}

static int IoTMsgProces(MQTTClient client, IoTMsgT* msg, MQTTClient_message pubMsg)
{
    int ret = 0;
    switch (msg->type) {
        case EN_IOT_MSG_PUBLISH:
            pubMsg.payload = (void*)msg->payload;
            pubMsg.payloadlen = (int)strlen(msg->payload);
            pubMsg.qos = msg->qos;
            pubMsg.retained = 0;
            ret = MQTTClient_publishMessage(client, msg->topic, &pubMsg, &g_ioTAppCb.tocken);
            if (ret != MQTTCLIENT_SUCCESS) {
                IOT_LOG_ERROR("MSGSEND:failed\r\n");
            }
            printf("MSGSEND:SUCCESS\r\n");
            g_ioTAppCb.tocken++;
            break;
        case EN_IOT_MSG_RECV:
            if (g_ioTAppCb.msgCallBack != NULL) {
                g_ioTAppCb.msgCallBack(msg->qos, msg->topic, msg->payload);
            }
            break;
        default:
            break;
    }
    return ret;
}

// use this function to deal all the comming message
static int ProcessQueueMsg(MQTTClient client)
{
    unsigned int ret;
    unsigned char msgSize;
    IoTMsgT* msg;
    unsigned int timeout;
    MQTTClient_message pubmsg = MQTTClient_message_initializer;

    timeout = CN_QUEUE_WAITTIMEOUT;
    do {
        msg = NULL;
        msgSize = sizeof(unsigned char);
        ret = osMessageQueueGet(g_ioTAppCb.queueID, &msg, &msgSize, timeout);
        if (msg != NULL) {
            printf("QUEUEMSG:QOS:%d TOPIC:%s PAYLOAD:%s\r\n", msg->qos, msg->topic, msg->payload);
            IoTMsgProces(client, msg, pubmsg);
            hi_free(0, msg);
        }
        timeout = 0; // < continous to deal the message without wait here
    } while (ret == IOT_SUCCESS);

    return ret;
}

volatile MQTTClient_deliveryToken deliveredtoken;
void delivered(void *context, MQTTClient_deliveryToken dt)
{
    (void) context;
    printf("Message with token value %d delivery confirmed\n", dt);
    deliveredtoken = dt;
}

void MqttProcess(MQTTClient client, char* clientID, char* userPwd, MQTTClient_connectOptions connOpts, int subQos[])
{
    int rc = MQTTClient_create(&client, CN_IOT_SERVER, clientID, MQTTCLIENT_PERSISTENCE_NONE, NULL);
    if (rc != MQTTCLIENT_SUCCESS) {
        IOT_LOG_ERROR("Create Client failed,Please check the parameters--%d\r\n", rc);
        if (userPwd != NULL) {
            hi_free(0, userPwd);
            return;
        }
    }
    rc = MQTTClient_setCallbacks(client, NULL, ConnLostCallBack, MsgRcvCallBack, delivered);
    if (rc != MQTTCLIENT_SUCCESS) {
        IOT_LOG_ERROR("Set the callback failed,Please check the callback paras\r\n");
        MQTTClient_destroy(&client);
        return;
    }

    rc = MQTTClient_connect(client, &connOpts);
    if (rc != MQTTCLIENT_SUCCESS) {
        IOT_LOG_ERROR("Connect IoT server failed,please check the network and parameters:%d\r\n", rc);
        MQTTClient_destroy(&client);
        return;
    }
    printf("Connect success\r\n");

    rc = MQTTClient_subscribeMany(client, CN_TOPIC_SUBSCRIBE_NUM, (char* const*)g_defaultSubscribeTopic,
                                  (int*)&subQos[0]);
    if (rc != MQTTCLIENT_SUCCESS) {
        IOT_LOG_ERROR("Subscribe the default topic failed,Please check the parameters\r\n");
        MQTTClient_destroy(&client);
        return;
    }
    printf("Subscribe success\r\n");
    while (MQTTClient_isConnected(client)) {
        ProcessQueueMsg(client);           // do the job here
        int ret = ProcessQueueMsg(client); // do the job here
        if (ret == HI_ERR_SUCCESS) {
            return;
        }
        MQTTClient_yield(); // make the keepalive done
    }
    MQTTClient_disconnect(client, CONFIG_COMMAND_TIMEOUT);
    return;
}

static void MainEntryProcess(void)
{
    int subQos[CN_TOPIC_SUBSCRIBE_NUM] = { 1 };

    MQTTClient client = NULL;
    MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
    // make the clientID userID userPwd
    char* clientID = CN_CLIENTID;
    char* userID = CONFIG_USER_ID;
    char* userPwd = hi_malloc(0, CN_HMAC_PWD_LEN);
    if (userPwd == NULL) {
        hi_free(0, clientID);
        return;
    }
    userPwd = CONFIG_USER_PWD;

    conn_opts.keepAliveInterval = CN_KEEPALIVE_TIME;
    conn_opts.cleansession = CN_CLEANSESSION;
    conn_opts.username = userID;
    conn_opts.password = userPwd;
    conn_opts.MQTTVersion = MQTTVERSION_3_1_1;
    // wait for the wifi connect ok
    printf("IOTSERVER:%s\r\n", CN_IOT_SERVER);
    MqttProcess(client, clientID, userPwd, conn_opts, subQos);
}

static void MainEntry(void)
{
    while (g_ioTAppCb.stop == HI_FALSE) {
        MainEntryProcess();
        printf("The connection lost and we will try another connect\r\n");
        hi_sleep(1000 * 5); /* 延时5*1000ms */
    }
}

int IoTMain(void)
{
   osThreadAttr_t attr;
    g_ioTAppCb.queueID = osMessageQueueNew(CN_QUEUE_MSGNUM, CN_QUEUE_MSGSIZE, NULL);
    attr.name = CN_TASK_NAME;
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024; /* 堆栈大小为1024; Stack size is 1024 */
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)MainEntry, NULL, &attr) == NULL) {
        printf("[StartPWMBeepTask] Falied to create PWMBeepTask!\n");
    }

    return 0;
}

int IoTSetMsgCallback(FnMsgCallBack msgCallback)
{
    g_ioTAppCb.msgCallBack = msgCallback;
    return 0;
}

int IotSendMsg(int qos, const char* topic, const char* payload)
{
    int rc = -1;
    IoTMsgT* msg;
    char* buf;
    hi_u32 bufSize;

    bufSize = strlen(topic) + 1 + strlen(payload) + 1 + sizeof(IoTMsgT);
    buf = hi_malloc(0, bufSize);
    if (buf != NULL) {
        msg = (IoTMsgT*)buf;
        buf += sizeof(IoTMsgT);
        bufSize -= sizeof(IoTMsgT);
        msg->qos = qos;
        msg->type = EN_IOT_MSG_PUBLISH;
        (void)memcpy_s(buf, bufSize, topic, strlen(topic));
        buf[strlen(topic)] = '\0';
        msg->topic = buf;
        buf += strlen(topic) + 1;
        bufSize -= (strlen(topic) + 1);
        (void)memcpy_s(buf, bufSize, payload, strlen(payload));
        buf[strlen(payload)] = '\0';
        msg->payload = buf;
        printf("SNDMSG:QOS:%d TOPIC:%s PAYLOAD:%s\r\n", msg->qos, msg->topic, msg->payload);
        if (osMessageQueuePut(g_ioTAppCb.queueID, &msg, 0, CN_QUEUE_WAITTIMEOUT) != IOT_SUCCESS) {
            IOT_LOG_ERROR("Wrie queue failed\r\n");
            hi_free(0, msg);
            return -1;
        } else {
            rc = 0;
        }
    }
    return rc;
}