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
#include "hal_bsp_aw2013.h"
#include "hal_bsp_structAll.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#include "cJSON.h"
#include "cmsis_os2.h"
#include "sys_config.h"

#define MQTT_RECV_TASK_TIME (200 * 1000) // us

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
        cJSON *service_id = cJSON_GetObjectItem(root, "service_id");
        if (!strcmp(service_id->valuestring, "control")) {
            cJSON *command_name = cJSON_GetObjectItem(root, "command_name");
            if (!strcmp(command_name->valuestring, "lamp")) {
                // 灯的手动控制
                cJSON *paras = cJSON_GetObjectItem(root, "paras");
                cJSON *value = cJSON_GetObjectItem(paras, "value");
                if (!strcmp(value->valuestring, "ON")) {
                    printf("command_name: lamp, value: ON.\r\n");
                    sys_msg_data.Lamp_Status = SUN_LIGHT_MODE;
                } else if (!strcmp(value->valuestring, "OFF")) {
                    printf("command_name: lamp, value: OFF.\r\n");
                    sys_msg_data.Lamp_Status = OFF_LAMP;
                }
                paras = value = NULL;
            }

            if (!strcmp(command_name->valuestring, "RGB")) {
                // RGB灯的颜色控制
                cJSON *paras = cJSON_GetObjectItem(root, "paras");
                cJSON *red = cJSON_GetObjectItem(paras, "red");
                cJSON *green = cJSON_GetObjectItem(paras, "green");
                cJSON *blue = cJSON_GetObjectItem(paras, "blue");

                sys_msg_data.RGB_Value.red = red->valueint;
                sys_msg_data.RGB_Value.green = green->valueint;
                sys_msg_data.RGB_Value.blue = blue->valueint;
                sys_msg_data.Lamp_Status = SET_RGB_MODE;
                printf("command_name: RGB, red: %d, green: %d, blue: %d.\r\n",
                       red->valueint,
                       green->valueint,
                       blue->valueint);
                paras = red = green = blue = NULL;
            }

            if (!strcmp(command_name->valuestring, "led_light")) {
                // 手动调节亮度
                cJSON *paras = cJSON_GetObjectItem(root, "paras");
                cJSON *value = cJSON_GetObjectItem(paras, "value");
                printf("command_name: led_light, value: ON.\r\n");
                sys_msg_data.led_light_value = value->valueint;
                paras = value = NULL;
            }

            /* 下面是自动控制的标志位 */
            if (!strcmp(command_name->valuestring, "is_auto_light_mode")) {
                // 是否开启自动亮度调节
                cJSON *paras = cJSON_GetObjectItem(root, "paras");
                cJSON *value = cJSON_GetObjectItem(paras, "value");

                if (!strcmp(value->valuestring, "ON")) {
                    printf("command_name: is_auto_light_mode, value: ON.\r\n");
                    sys_msg_data.is_auto_light_mode = 1;
                } else if (!strcmp(value->valuestring, "OFF")) {
                    printf("command_name: is_auto_light_mode, value: OFF.\r\n");
                    sys_msg_data.is_auto_light_mode = 0;
                }
                paras = value = NULL;
            }

            if (!strcmp(command_name->valuestring, "is_sleep_mode")) {
                // 是否开启睡眠模式
                cJSON *paras = cJSON_GetObjectItem(root, "paras");
                cJSON *value = cJSON_GetObjectItem(paras, "value");

                if (!strcmp(value->valuestring, "ON")) {
                    printf("command_name: is_sleep_mode, value: ON.\r\n");
                    sys_msg_data.Lamp_Status = SLEEP_MODE; // 睡眠模式
                } else if (!strcmp(value->valuestring, "OFF")) {
                    printf("command_name: is_sleep_mode, value: OFF.\r\n");
                    sys_msg_data.Lamp_Status = OFF_LAMP; // 关闭灯光
                }
                paras = value = NULL;
            }

            if (!strcmp(command_name->valuestring, "is_readbook_mode")) {
                // 是否开启阅读模式
                cJSON *paras = cJSON_GetObjectItem(root, "paras");
                cJSON *value = cJSON_GetObjectItem(paras, "value");

                if (!strcmp(value->valuestring, "ON")) {
                    printf("command_name: is_readbook_mode, value: ON.\r\n");
                    sys_msg_data.Lamp_Status = READ_BOOK_MODE; // 阅读模式
                } else if (!strcmp(value->valuestring, "OFF")) {
                    printf("command_name: is_readbook_mode, value: OFF.\r\n");
                    sys_msg_data.Lamp_Status = OFF_LAMP; // 关闭灯光
                }
                paras = value = NULL;
            }

            if (!strcmp(command_name->valuestring, "is_blink_mode")) {
                // 是否开启闪烁模式
                cJSON *paras = cJSON_GetObjectItem(root, "paras");
                cJSON *value = cJSON_GetObjectItem(paras, "value");

                if (!strcmp(value->valuestring, "ON")) {
                    printf("command_name: is_blink_mode, value: ON.\r\n");
                    sys_msg_data.Lamp_Status = LED_BLINK_MODE; // 阅读模式
                } else if (!strcmp(value->valuestring, "OFF")) {
                    printf("command_name: is_blink_mode, value: OFF.\r\n");
                    sys_msg_data.Lamp_Status = OFF_LAMP; // 关闭灯光
                }
                paras = value = NULL;
            }
            command_name = NULL;
        }
        service_id = NULL;
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
                                            strlen(DEVICE_ID) + strlen(request_id) + 1);
        if (request_topic != NULL) {
            memset_s(request_topic,
                     strlen(DEVICE_ID) + strlen(MALLOC_MQTT_TOPIC_PUB_COMMANDS_REQ) + strlen(request_id) + 1,
                     0,
                     strlen(DEVICE_ID) + strlen(MALLOC_MQTT_TOPIC_PUB_COMMANDS_REQ) + strlen(request_id) + 1);
            sprintf_s(request_topic,
                      strlen(DEVICE_ID) + strlen(MALLOC_MQTT_TOPIC_PUB_COMMANDS_REQ) + strlen(request_id) + 1,
                      MQTT_TOPIC_PUB_COMMANDS_REQ, DEVICE_ID, request_id);
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
        usleep(MQTT_RECV_TASK_TIME);
    }
}
