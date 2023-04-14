/*
 * Copyright (c) 2023 Beijing HuaQing YuanJian Education Technology Co., Ltd
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "hal_bsp_wifi.h"
#include "hal_bsp_mqtt.h"
#include "hal_bsp_ap3216c.h"
#include "hal_bsp_aw2013.h"
#include "hal_bsp_pcf8574.h"
#include "hal_bsp_sht20.h"
#include "hal_bsp_ssd1306.h"
#include "hal_bsp_structAll.h"

#include "cJSON.h"

// 设备密码 fs12345678
// 设备ID
#define DEVICE_ID "643641d692edbc7ee93ecdd9_board"
// MQTT客户端ID
#define MQTT_CLIENT_ID "643641d692edbc7ee93ecdd9_board_0_0_2023041205"
// MQTT用户名
#define MQTT_USER_NAME "643641d692edbc7ee93ecdd9_board"
// MQTT密码
#define MQTT_PASS_WORD "52bc6b65f3db62f9e456b4a4a031cacfe34f865cc69f457de8a8a67b9b963b6f"
// 华为云平台的IP地址
#define SERVER_IP_ADDR "121.36.42.100"
// 华为云平台的IP端口号
#define SERVER_IP_PORT 1883
// 订阅 接收控制命令的主题
#define MQTT_TOPIC_SUB_COMMANDS "$oc/devices/%s/sys/commands/#"
// 发布 成功接收到控制命令后的主题
#define MQTT_TOPIC_PUB_COMMANDS_REQ "$oc/devices/%s/sys/commands/response/request_id=%s"
#define MALLOC_MQTT_TOPIC_PUB_COMMANDS_REQ "$oc/devices//sys/commands/response/request_id="

// 发布 设备属性数据的主题
#define MQTT_TOPIC_PUB_PROPERTIES "$oc/devices/%s/sys/properties/report"
#define MALLOC_MQTT_TOPIC_PUB_PROPERTIES "$oc/devices//sys/properties/report"
#define MQTT_PAYLOAD_PUB "{\"services\":[{\"service_id\":\"attribute\",\"properties\": \
\"{\"buzzer\":\"%s\",\"fan\":\"%s\",\"led\":\"%s\",\"temperature\":%.1f,\"humidity\":%.1f, \
\"light\":%d,\"proximity\":%d,\"infrared\":%d}}]}"

#define TASK_STACK_SIZE (1024 * 10)
#define MsgQueueObjectNumber 16 // 定义消息队列对象的个数
typedef struct message_sensorData {
    uint8_t led;        // LED灯当前的状态
    uint8_t fan;        // 风扇当前的状态
    uint8_t buzzer;     // 蜂鸣器当前的状态
    uint16_t light;     // 光照强度
    uint16_t proximity; // 接近传感器的数值
    uint16_t infrared;  // 人体红外传感器的数值
    float temperature;  // 当前的温度值
    float humidity;     // 当前的湿度值
} msg_sensorData_t;
msg_sensorData_t sensorData = {0}; // 传感器的数据
osThreadId_t mqtt_send_task_id; // mqtt 发布数据任务ID
osThreadId_t mqtt_recv_task_id; // mqtt 接收数据任务ID
#define MQTT_SEND_TASK_TIME 3 // s
#define MQTT_RECV_TASK_TIME 1 // s
#define TASK_INIT_TIME 2 // s
#define DISPLAY_BUFF_MAX 64
#define MQTT_TOPIC_MAX 128
uint8_t publish_topic[MQTT_TOPIC_MAX] = {0};
uint8_t displayBuffer[DISPLAY_BUFF_MAX] = {0};

/**
 * @brief MQTT  发布消息任务
 */
void mqtt_send_task(void)
{
    cJSON *root = NULL, *array = NULL, *services = NULL;
    cJSON *properties = NULL;
    
    while (1) {
        // 获取传感器的数据
        SHT20_ReadData(&sensorData.temperature, &sensorData.humidity);
        AP3216C_ReadData(&sensorData.infrared, &sensorData.proximity, &sensorData.light);

        memset_s(displayBuffer, DISPLAY_BUFF_MAX, 0, DISPLAY_BUFF_MAX);
        sprintf_s((char *)displayBuffer, DISPLAY_BUFF_MAX, "T:%.1fC H:%.1f%%",
                  sensorData.temperature, sensorData.humidity);
        SSD1306_ShowStr(OLED_TEXT16_COLUMN_0, OLED_TEXT16_LINE_0, displayBuffer, TEXT_SIZE_16);

        memset_s(displayBuffer, DISPLAY_BUFF_MAX, 0, DISPLAY_BUFF_MAX);
        sprintf_s((char *)displayBuffer, DISPLAY_BUFF_MAX, "ir:%04d", sensorData.infrared);
        SSD1306_ShowStr(OLED_TEXT16_COLUMN_0, OLED_TEXT16_LINE_1, displayBuffer, TEXT_SIZE_16);

        memset_s(displayBuffer, DISPLAY_BUFF_MAX, 0, DISPLAY_BUFF_MAX);
        sprintf_s((char *)displayBuffer, DISPLAY_BUFF_MAX, "ps:%04d", sensorData.proximity);
        SSD1306_ShowStr(OLED_TEXT16_COLUMN_0, OLED_TEXT16_LINE_2, displayBuffer, TEXT_SIZE_16);

        memset_s(displayBuffer, DISPLAY_BUFF_MAX, 0, DISPLAY_BUFF_MAX);
        sprintf_s((char *)displayBuffer, DISPLAY_BUFF_MAX, "Lux:%04d", sensorData.light);
        SSD1306_ShowStr(OLED_TEXT16_COLUMN_0, OLED_TEXT16_LINE_3, displayBuffer, TEXT_SIZE_16);

        // 组Topic
        memset_s(publish_topic, MQTT_TOPIC_MAX, 0, MQTT_TOPIC_MAX);
        if (sprintf_s(publish_topic, MQTT_TOPIC_MAX, MQTT_TOPIC_PUB_PROPERTIES, DEVICE_ID) > 0) {
            // 组JSON数据
            root = cJSON_CreateObject(); // 创建一个对象
            services = cJSON_CreateArray();
            cJSON_AddItemToObject(root, "services", services);
            array = cJSON_CreateObject();
            cJSON_AddStringToObject(array, "service_id", "attribute");
            properties = cJSON_CreateObject();
            cJSON_AddItemToObject(array, "properties", properties);
            cJSON_AddStringToObject(properties, "buzzer", sensorData.buzzer ? "ON" : "OFF");
            cJSON_AddStringToObject(properties, "fan", sensorData.fan ? "ON" : "OFF");
            cJSON_AddStringToObject(properties, "led", sensorData.led ? "ON" : "OFF");
            cJSON_AddNumberToObject(properties, "humidity", (int)sensorData.humidity);
            cJSON_AddNumberToObject(properties, "temperature", (int)sensorData.temperature);
            cJSON_AddNumberToObject(properties, "light", sensorData.light);
            cJSON_AddNumberToObject(properties, "proximity", sensorData.proximity);
            cJSON_AddNumberToObject(properties, "infrared", sensorData.infrared);
            cJSON_AddItemToArray(services, array);  // 将对象添加到数组中

            /* 格式化打印创建的带数组的JSON对象 */
            char *str_print = cJSON_PrintUnformatted(root);
            if (str_print != NULL) {
                // printf("%s\n", str_print);
                // 发布消息
                MQTTClient_pub(publish_topic, str_print, strlen((char *)str_print));
                cJSON_free(str_print);
            }

            if (root != NULL) {
                cJSON_Delete(root);
            }

            properties = str_print = root = array = services = NULL;
        }
    }
    sleep(MQTT_SEND_TASK_TIME);
}

/**
 * @brief MQTT接收数据的回调函数
 */
int8_t mqttClient_sub_callback(unsigned char *topic, unsigned char *payload)
{
    if ((topic == NULL) || (payload == NULL)) {
        return -1;
    } else {
        printf("topic: %s\r\n", topic);
        printf("payload: %s\r\n", payload);

        // 提取出topic中的request_id
        char request_id[50] = {0};
        int ret_code = 1; // 1为失败
        int ret = -1;
        ret = strcpy_s(request_id, sizeof(request_id),
                       topic + strlen(DEVICE_ID) + strlen("$oc/devices//sys/commands/request_id="));
        if (ret == 0) {
            printf("request_id: %s\r\n", request_id);

            cJSON *root = NULL;
            cJSON *command_name = NULL;
            cJSON *paras = NULL;
            cJSON *value = NULL;
            cJSON *red = NULL;
            cJSON *green = NULL;
            cJSON *blue = NULL;

            root = cJSON_Parse((const char *)payload);
            if (root) {
                // 解析JSON数据
                command_name = cJSON_GetObjectItem(root, "command_name");
                paras = cJSON_GetObjectItem(root, "paras");
                if (command_name) {
                    if (!strcmp(command_name->valuestring, "led")) {
                        value = cJSON_GetObjectItem(paras, "value");
                        if (!strcmp(value->valuestring, "ON")) {
                            printf("led on\r\n");
                            set_led(true);
                            sensorData.led = 1;

                            ret_code = 0; // 0为成功
                        } else if (!strcmp(value->valuestring, "OFF")) {
                            printf("led off\r\n");
                            set_led(false);
                            sensorData.led = 0;

                            ret_code = 0; // 0为成功
                        }
                    } else if (!strcmp(command_name->valuestring, "fan")) {
                        value = cJSON_GetObjectItem(paras, "value");
                        if (!strcmp(value->valuestring, "ON")) {
                            printf("fan on\r\n");
                            set_fan(true);
                            sensorData.fan = 1;

                            ret_code = 0; // 0为成功
                        } else if (!strcmp(value->valuestring, "OFF")) {
                            printf("fan off\r\n");
                            set_fan(false);
                            sensorData.fan = 0;

                            ret_code = 0; // 0为成功
                        }
                    } else if (!strcmp(command_name->valuestring, "buzzer")) {
                        value = cJSON_GetObjectItem(paras, "value");
                        if (!strcmp(value->valuestring, "ON")) {
                            printf("buzzer on\r\n");
                            set_buzzer(true);
                            sensorData.buzzer = 1;

                            ret_code = 0; // 0为成功
                        } else if (!strcmp(value->valuestring, "OFF")) {
                            printf("buzzer off\r\n");
                            set_buzzer(false);
                            sensorData.buzzer = 0;

                            ret_code = 0; // 0为成功
                        }
                    } else if (!strcmp(command_name->valuestring, "RGB")) {
                        red = cJSON_GetObjectItem(paras, "red");
                        green = cJSON_GetObjectItem(paras, "green");
                        blue = cJSON_GetObjectItem(paras, "blue");

                        printf("red:%d  green:%d  blue:%d\r\n", red->valueint, green->valueint, blue->valueint);
                        AW2013_Control_Red(red->valueint);
                        AW2013_Control_Green(green->valueint);
                        AW2013_Control_Blue(blue->valueint);
                        ret_code = 0; // 0为成功
                    }
                }

                // 向云端发送命令设置的返回值
                char *request_topic = (char *)malloc(strlen(MALLOC_MQTT_TOPIC_PUB_COMMANDS_REQ) +
                                                    strlen(DEVICE_ID) + sizeof(request_id) + 1);
                if (request_topic != NULL) {
                    memset_s(request_topic,
                            strlen(DEVICE_ID) + strlen(MALLOC_MQTT_TOPIC_PUB_COMMANDS_REQ) + sizeof(request_id) + 1,
                            0,
                            strlen(DEVICE_ID) + strlen(MALLOC_MQTT_TOPIC_PUB_COMMANDS_REQ) + sizeof(request_id) + 1);
                    sprintf_s(request_topic,
                            strlen(DEVICE_ID) + strlen(MALLOC_MQTT_TOPIC_PUB_COMMANDS_REQ) + sizeof(request_id) + 1,
                            MQTT_TOPIC_PUB_COMMANDS_REQ, DEVICE_ID, request_id);

                    if (ret_code == 0) {
                        MQTTClient_pub(request_topic, "{\"result_code\":0}", strlen("{\"result_code\":0}"));
                    } else if (ret_code == 1) {
                        MQTTClient_pub(request_topic, "{\"result_code\":1}", strlen("{\"result_code\":1}"));
                    }
                    free(request_topic);
                }
            }
            root = NULL;
            command_name = NULL;
            paras = NULL;
            value = NULL;
            red = NULL;
            green = NULL;
            blue = NULL;
        }
    }
    return 0;
}

/**
 * @brief MQTT  接收消息任务
 */
void mqtt_recv_task(void)
{
    while (1) {
        MQTTClient_sub();
        sleep(MQTT_RECV_TASK_TIME);
    }
}

static void network_wifi_mqtt_example(void)
{
    printf("Enter network_wifi_mqtt_example()!\r\n");

    // 外设的初始化
    PCF8574_Init();
    AW2013_Init(); // 三色LED灯的初始化
    AW2013_Control_Red(0);
    AW2013_Control_Green(0);
    AW2013_Control_Blue(0);
    SHT20_Init();   // SHT20初始化
    AP3216C_Init(); // 三合一传感器初始化
    SSD1306_Init(); // OLED 显示屏初始化
    SSD1306_CLS();  // 清屏

    p_MQTTClient_sub_callback = &mqttClient_sub_callback;

    // 连接WiFi
    if (WiFi_connectHotspots("AI_DEV", "HQYJ12345678") != WIFI_SUCCESS) {
        printf("[error] connectWiFiHotspots\r\n");
    } else {
        printf("[success] connectWiFiHotspots");
    }
    sleep(TASK_INIT_TIME);

    // 连接MQTT服务器
    if (MQTTClient_connectServer(SERVER_IP_ADDR, SERVER_IP_PORT) != WIFI_SUCCESS) {
        printf("[error] mqttClient_connectServer\r\n");
    } else {
        printf("[success] mqttClient_connectServer");
    }
    sleep(TASK_INIT_TIME);

    // 初始化MQTT客户端
    if (MQTTClient_init(MQTT_CLIENT_ID, MQTT_USER_NAME, MQTT_PASS_WORD) != WIFI_SUCCESS) {
        printf("[error] mqttClient_init\r\n");
    } else {
        printf("[success] mqttClient_init");
    }
    sleep(TASK_INIT_TIME);

    // 订阅主题
    if (MQTTClient_subscribe(MQTT_TOPIC_SUB_COMMANDS) != WIFI_SUCCESS) {
        printf("[error] mqttClient_subscribe\r\n");
    } else {
        printf("[success] mqttClient_subscribe\r\n");
    }
    sleep(TASK_INIT_TIME);

    //  创建线程
    osThreadAttr_t options;
    options.name = "mqtt_send_task";
    options.attr_bits = 0;
    options.cb_mem = NULL;
    options.cb_size = 0;
    options.stack_mem = NULL;
    options.stack_size = TASK_STACK_SIZE;
    options.priority = osPriorityNormal;
    mqtt_send_task_id = osThreadNew((osThreadFunc_t)mqtt_send_task, NULL, &options);
    if (mqtt_send_task_id != NULL) {
        printf("ID = %d, Create mqtt_send_task_id is OK!\r\n", mqtt_send_task_id);
    }

    options.name = "mqtt_recv_task";
    options.stack_size = TASK_STACK_SIZE;
    mqtt_recv_task_id = osThreadNew((osThreadFunc_t)mqtt_recv_task, NULL, &options);
    if (mqtt_recv_task_id != NULL) {
        printf("ID = %d, Create mqtt_recv_task_id is OK!\r\n", mqtt_recv_task_id);
    }
}
SYS_RUN(network_wifi_mqtt_example);
