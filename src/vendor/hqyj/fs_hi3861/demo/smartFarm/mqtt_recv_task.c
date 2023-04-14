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
#include "hi_nv.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "cJSON.h"
#include "cmsis_os2.h"
#include "sys_config.h"

#define MQTT_RECV_TASK_DELAY_TIME (100 * 1000) // us

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
                    if (!strcmp(json_command_name->valuestring, "fan")) {
                        cJSON *json_value = cJSON_GetObjectItem(cJSON_GetObjectItem(root, "paras"), "value");
                        if (json_value) {
                            if (!strcmp(json_value->valuestring, "ON")) {
                                printf("command_name: fan, value: ON.\r\n");
                                sys_msg_data.fanStatus = 1;
                            } else if (!strcmp(json_value->valuestring, "OFF")) {
                                printf("command_name: fan, value: OFF.\r\n");
                                sys_msg_data.fanStatus = 0;
                            }
                        }
                        json_value = NULL;
                    }

                    // 接收自动控制命令
                    if (!strcmp(json_command_name->valuestring, "autoMode")) {
                        cJSON *json_value = cJSON_GetObjectItem(cJSON_GetObjectItem(root, "paras"), "value");
                        if (json_value) {
                            if (!strcmp(json_value->valuestring, "ON")) {
                                printf("command_name: autoMode, value: ON.\r\n");
                                sys_msg_data.nvFlash.smartControl_flag = 1;
                            } else if (!strcmp(json_value->valuestring, "OFF")) {
                                printf("command_name: autoMode, value: OFF.\r\n");
                                sys_msg_data.nvFlash.smartControl_flag = 0;
                            }
                            // NV值写入
                            // hi_factory_nv_write(NV_ID, &sys_msg_data.nvFlash,
                            //                     sizeof(hi_nv_save_sensor_threshold), 0);
                        }
                        json_value = NULL;
                    }

                    // 接收湿度的上限和下限值
                    if (!strcmp(json_command_name->valuestring, "humidity")) {
                        cJSON *json_up = cJSON_GetObjectItem(cJSON_GetObjectItem(root, "paras"), "up");
                        if (json_up) {
                            printf("command_name: humidity, up: %d.\r\n", json_up->valueint);
                            sys_msg_data.nvFlash.humi_upper = json_up->valueint;
                        }
                        json_up = NULL;

                        cJSON *json_down = cJSON_GetObjectItem(cJSON_GetObjectItem(root, "paras"), "down");
                        if (json_down) {
                            printf("command_name: humidity, down: %d.\r\n", json_down->valueint);
                            sys_msg_data.nvFlash.humi_lower = json_down->valueint;
                        }
                        json_down = NULL;

                        // NV值写入
                        // hi_factory_nv_write(NV_ID, &sys_msg_data.nvFlash, sizeof(hi_nv_save_sensor_threshold), 0);
                    }
                }
                json_command_name = NULL;
            }
        }
        json_service_id = NULL;
    }

    cJSON_Delete(root);
    root = NULL;

    return 0;
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
        strcpy_s(request_id, sizeof(request_id),
                topic + strlen(DEVICE_ID) + strlen("$oc/devices//sys/commands/request_id="));
        printf("request_id: %s\r\n", request_id);

        // 解析JSON数据并控制
        ret_code = cJSON_Parse_Payload(payload);

        // 向云端发送命令设置的返回值
        char *request_topic = (char *)malloc(strlen(MALLOC_MQTT_TOPIC_PUB_COMMANDS_REQ) + \
                                            strlen(DEVICE_ID) + strlen(request_id));
        if (request_topic != NULL) {
            memset_s(request_topic,
                     strlen(DEVICE_ID) + strlen(MALLOC_MQTT_TOPIC_PUB_COMMANDS_REQ) + strlen(request_id) + 1,
                     0,
                     strlen(DEVICE_ID) + strlen(MALLOC_MQTT_TOPIC_PUB_COMMANDS_REQ) + strlen(request_id) + 1);
            sprintf_s(request_topic,
                      strlen(DEVICE_ID) + strlen(MALLOC_MQTT_TOPIC_PUB_COMMANDS_REQ) + strlen(request_id) + 1,
                      MQTT_TOPIC_PUB_COMMANDS_REQ, DEVICE_ID, request_id);
            // printf("topic: %s\r\n", request_topic);
            if (ret_code == 0) {
                MQTTClient_pub(request_topic, "{\"result_code\":0}", strlen("{\"result_code\":0}"));
            } else if (ret_code == 1) {
                MQTTClient_pub(request_topic, "{\"result_code\":1}", strlen("{\"result_code\":1}"));
            }
            free(request_topic);
        }
        request_topic = NULL;
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
        usleep(MQTT_RECV_TASK_DELAY_TIME);
    }
}
