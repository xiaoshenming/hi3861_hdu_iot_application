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

#include "udp_recv_task.h"
#include "udp_send_task.h"

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "hi_uart.h"

#include "sys_config.h"

#include "lwip/netifapi.h"
#include "lwip/sockets.h"
#include "lwip/api_shell.h"

#include "cJSON.h"

char udp_recvBuff[512] = {0};                    // 数据缓冲区
char uart_sendBuff[128] = {0};                   // 发送数据缓冲区
uint16_t L_PWM_Value = 350, R_PWM_Value = 350;   // 默认的PWM参数值
uint16_t base_pwm_speed_value = MOTOR_LOW_SPEED; // 速度倍率

void udp_recv_task(void)
{
    socklen_t len = sizeof(client);
    osStatus_t msgStatus;
    while (1) {
        ssize_t client_data = recvfrom(systemValue.udp_socket_fd, udp_recvBuff, sizeof(udp_recvBuff) - 1,
                                       0, (struct sockaddr *)&client, &len);
        if (client_data > 0) {
            // 读成功
            printf("udp recv data is \" %s \".\r\n", udp_recvBuff);
            /* 解析JSON数据 */
            cJSON *root = cJSON_Parse(udp_recvBuff);
            if (root) {
                cJSON *json_carSpeed = cJSON_GetObjectItem(root, "carSpeed");
                if (json_carSpeed != NULL) {
                    printf("carSpeed: %s\r\n", json_carSpeed->valuestring);
                    if (!strcmp(json_carSpeed->valuestring, "low")) {
                        systemValue.car_status = CAR_STATUS_L_SPEED;
                        base_pwm_speed_value = MOTOR_LOW_SPEED;
                    } else if (!strcmp(json_carSpeed->valuestring, "middle")) {
                        systemValue.car_status = CAR_STATUS_M_SPEED;
                        base_pwm_speed_value = MOTOR_MIDDLE_SPEED;
                    } else if (!strcmp(json_carSpeed->valuestring, "high")) {
                        systemValue.car_status = CAR_STATUS_H_SPEED;
                        base_pwm_speed_value = MOTOR_HIGH_SPEED;
                    }
                    json_carSpeed = NULL;
                }

                cJSON *json_autoMode = cJSON_GetObjectItem(root, "autoMode");
                if (json_autoMode != NULL) {
                    if (json_autoMode->valueint == 0) {
                        systemValue.auto_abstacle_flag = 0;
                    } else {
                        systemValue.auto_abstacle_flag = 1;
                    }
                }

                cJSON *json_carStatus = cJSON_GetObjectItem(root, "carStatus");
                if (json_carStatus != NULL) {
                    printf("carStatus: %s\r\n", json_carStatus->valuestring);
                    if (!strcmp(json_carStatus->valuestring, "on")) {
                        systemValue.car_status = CAR_STATUS_ON;
                        memset_s(uart_sendBuff, sizeof(uart_sendBuff), 0, sizeof(uart_sendBuff));
                        sprintf_s(uart_sendBuff, sizeof(uart_sendBuff), "{\"control\":{\"power\":\"on\"}}");
                        uart_send_buff(uart_sendBuff, strlen(uart_sendBuff));
                    } else if (!strcmp(json_carStatus->valuestring, "off")) {
                        systemValue.car_status = CAR_STATUS_OFF;
                        memset_s(uart_sendBuff, sizeof(uart_sendBuff), 0, sizeof(uart_sendBuff));
                        sprintf_s(uart_sendBuff, sizeof(uart_sendBuff), "{\"control\":{\"power\":\"off\"}}");
                        uart_send_buff(uart_sendBuff, strlen(uart_sendBuff));
                    } else if (!strcmp(json_carStatus->valuestring, "run")) {
                        systemValue.car_status = CAR_STATUS_RUN;
                        memset_s(uart_sendBuff, sizeof(uart_sendBuff), 0, sizeof(uart_sendBuff));
                        sprintf_s(uart_sendBuff, sizeof(uart_sendBuff),
                                "{\"control\":{\"turn\":\"run\",\"pwm\":{\"L_Motor\":%d,\"R_Motor\":%d}}}",
                                base_pwm_speed_value + L_PWM_Value,
                                base_pwm_speed_value + R_PWM_Value);
                        uart_send_buff(uart_sendBuff, strlen(uart_sendBuff));
                    } else if (!strcmp(json_carStatus->valuestring, "back")) {
                        systemValue.car_status = CAR_STATUS_BACK;
                        memset_s(uart_sendBuff, sizeof(uart_sendBuff), 0, sizeof(uart_sendBuff));
                        sprintf_s(uart_sendBuff, sizeof(uart_sendBuff),
                                "{\"control\":{\"turn\":\"back\",\"pwm\":{\"L_Motor\":%d,\"R_Motor\":%d}}}",
                                base_pwm_speed_value + L_PWM_Value,
                                base_pwm_speed_value + R_PWM_Value);
                        uart_send_buff(uart_sendBuff, strlen(uart_sendBuff));
                    } else if (!strcmp(json_carStatus->valuestring, "left")) {
                        systemValue.car_status = CAR_STATUS_LEFT;
                        memset_s(uart_sendBuff, sizeof(uart_sendBuff), 0, sizeof(uart_sendBuff));
                        sprintf_s(uart_sendBuff, sizeof(uart_sendBuff),
                                "{\"control\":{\"turn\":\"left\",\"pwm\":{\"L_Motor\":%d,\"R_Motor\":%d}}}",
                                L_PWM_Value,
                                R_PWM_Value);
                        uart_send_buff(uart_sendBuff, strlen(uart_sendBuff));
                    } else if (!strcmp(json_carStatus->valuestring, "right")) {
                        systemValue.car_status = CAR_STATUS_RIGHT;
                        memset_s(uart_sendBuff, sizeof(uart_sendBuff), 0, sizeof(uart_sendBuff));
                        sprintf_s(uart_sendBuff, sizeof(uart_sendBuff),
                                "{\"control\":{\"turn\":\"right\",\"pwm\":{\"L_Motor\":%d,\"R_Motor\":%d}}}",
                                L_PWM_Value,
                                R_PWM_Value);

                        uart_send_buff(uart_sendBuff, strlen(uart_sendBuff));
                    } else if (!strcmp(json_carStatus->valuestring, "stop")) {
                        systemValue.car_status = CAR_STATUS_STOP;
                        memset_s(uart_sendBuff, sizeof(uart_sendBuff), 0, sizeof(uart_sendBuff));
                        sprintf_s(uart_sendBuff, sizeof(uart_sendBuff), "{\"control\":{\"turn\":\"stop\"}}");
                        uart_send_buff(uart_sendBuff, strlen(uart_sendBuff));
                    }
                    json_carStatus = NULL;
                }
            }
            cJSON_Delete(root);
            root = NULL;
            memset_s(udp_recvBuff, sizeof(udp_recvBuff), 0, sizeof(udp_recvBuff));
        }
    }
    closesocket(systemValue.udp_socket_fd);
}
