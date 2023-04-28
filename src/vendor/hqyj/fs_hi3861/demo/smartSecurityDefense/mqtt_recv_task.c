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

#include "mqtt_recv_task.h"
#include "hal_bsp_mqtt.h"
#include "hal_bsp_structAll.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "cJSON.h"
#include "cmsis_os2.h"
#include "sys_config.h"

#define MQTT_RECV_TASK_TIME (200 * 1000) // us

int get_jsonData_value(const cJSON *const object, uint8_t *value)
{
    cJSON *json_value = NULL;
    json_value = cJSON_GetObjectItem(object, "value");
    if (json_value) {
       if (!strcmp(json_value->valuestring, "ON")) {
            *value = 1;
            json_value = NULL;
            return 0; // 0为成功
        } else if (!strcmp(json_value->valuestring, "OFF")) {
            *value = 0;
            json_value = NULL;
            return 0; 
        } 
    }
    json_value = NULL;
    return -1; // -1为失败
}
// 解析JSON数据
uint8_t cJSON_Parse_Payload(uint8_t *payload)
{
    uint8_t ret = 0;
    if (payload == NULL) {
        printf("payload is NULL\r\n");
        return 1;
    }

    cJSON *root = cJSON_Parse((const char *)payload);
    if (root) {
        cJSON *json_service_id = cJSON_GetObjectItem(root, "service_id");
        if (json_service_id) {
            if (!strcmp(json_service_id->valuestring, "control")) {
                cJSON *json_command_name = cJSON_GetObjectItem(root, "command_name");
                if (json_command_name) {
                    // 接收风扇控制命令
                    if (!strcmp(json_command_name->valuestring, "buzzer")) {
                        cJSON *paras = cJSON_GetObjectItem(root, "paras");
                        ret = get_jsonData_value(paras, &sys_msg_data.buzzerStatus);
                    }

                    // 接收自动控制命令
                    if (!strcmp(json_command_name->valuestring, "autoMode")) {
                        cJSON *paras = cJSON_GetObjectItem(root, "paras");
                        ret = get_jsonData_value(paras, &sys_msg_data.nvFlash.smartControl_flag);
                    }
                }
                json_command_name = NULL;
            }
        }
        json_service_id = NULL;
    }
    cJSON_Delete(root);
    root = NULL;
    return ret;
}
// 向云端发送返回值
void send_cloud_request_code(const char *request_id, int ret_code)
{
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
        request_topic = NULL;
    }
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
        int ret_code = 1; // 0为成功, 其余为失败。不带默认表示成功
        if (0 != strcpy_s(request_id, sizeof(request_id),
                          topic + strlen(DEVICE_ID) + strlen("$oc/devices//sys/commands/request_id="))) {
            return -1;
        }
        printf("request_id: %s\r\n", request_id);

        // 解析JSON数据并控制
        ret_code = cJSON_Parse_Payload(payload);
        send_cloud_request_code(request_id, ret_code);
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
        usleep(MQTT_RECV_TASK_TIME);
    }
}
