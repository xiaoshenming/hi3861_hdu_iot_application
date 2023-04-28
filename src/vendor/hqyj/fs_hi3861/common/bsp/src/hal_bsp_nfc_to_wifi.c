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
#include "cJSON.h"

#include "hal_bsp_nfc.h"
#include "hal_bsp_wifi.h"
#include "hal_bsp_nfc_to_wifi.h"

#define MAX_BUFF 64
#define OFFSET_HEAD 5 // 头部字节偏移
#define OFFSET_WIFI_NAME 12 // 从数据类型的尾部到WiFi名称数据长度的偏移
#define OFFSET_WIFI_PASSWD 17 // 从WiFi名称数据的尾部到WiFi密码数据长度的偏移
/**
 * @brief  使用NFC进行配网
 * @note   驱动NDEF协议中的第一个标签数据，然后进行配网
 *         配合《NFC调试文档.xlsx》，看下面的程序，更容易看懂
 * @param  *ndefBuff: 标签数据的缓冲区
 * @retval
 */
uint32_t NFC_configuresWiFiNetwork(uint8_t *ndefBuff)
{
    if (ndefBuff == NULL) {
        printf("NFC_configuresWiFiNetwork to ndefBuff is NULL\r\n");
        return HI_ERR_FAILURE;
    }
    uint8_t ret = 0;
    uint8_t wifi_name[MAX_BUFF] = {0}; // WiFi名称
    uint8_t wifi_passwd[MAX_BUFF] = {0}; // WiFi密码
    uint8_t dataType_len = ndefBuff[NDEF_PROTOCOL_DATA_TYPE_LENGTH_OFFSET]; // 获取数据类型长度
    uint8_t payload_len = ndefBuff[NDEF_PROTOCOL_DATA_LENGTH_OFFSET]; // 获取数据长度
    uint8_t *dataType_buff = (uint8_t *)malloc(dataType_len + 1);

    if (dataType_buff == NULL) {
        printf("dataType_buff malloc failed.\r\n");
        return HI_ERR_FAILURE;
    } else {
        memset_s(dataType_buff, dataType_len + 1, 0, dataType_len + 1);
        // ndefBuff + OFFSET_HEAD，代表数组地址偏移5个字节
        if (0 == memcpy_s(dataType_buff, dataType_len + 1, ndefBuff + OFFSET_HEAD, dataType_len)) {
            printf("Use AppleIOS system..........\n");
            printf("dataType: %s\n", dataType_buff);
            // 使用IOS系统的 NFC Tools软件进行配网
            if (strcmp(dataType_buff, "application/vnd.wfa.wsc") == 0) {
                // 判断是不是WiFi配网数据
                uint8_t wifi_name_len = ndefBuff[OFFSET_HEAD + dataType_len + OFFSET_WIFI_NAME];
                if (0 != memcpy_s(wifi_name, MAX_BUFF, 
                                  ndefBuff + OFFSET_HEAD + dataType_len + OFFSET_WIFI_NAME + 1, wifi_name_len)) {
                    free(dataType_buff);
                    dataType_buff = NULL;
                    return HI_ERR_FAILURE;
                }
                uint8_t wifi_passwd_len = ndefBuff[OFFSET_HEAD + dataType_len + OFFSET_WIFI_NAME + 1\
                                                + wifi_name_len + OFFSET_WIFI_PASSWD];
                if (0 != memcpy_s(wifi_passwd, MAX_BUFF, 
                                  ndefBuff + OFFSET_HEAD + dataType_len + OFFSET_WIFI_NAME + \
                                  wifi_name_len + OFFSET_WIFI_PASSWD, wifi_passwd_len)) {
                    free(dataType_buff);
                    dataType_buff = NULL;
                    return HI_ERR_FAILURE;
                }
                ret = 1; // 成功获取到WiFi名称和密码
            } else {
                ret = 0;
            }
        }
    }
    free(dataType_buff);
    dataType_buff = NULL;

    // 使用微信小程序进行配网
    if ((ret == 0) && (ndefBuff[NDEF_PROTOCOL_DATA_TYPE_OFFSET] == 't')) {
        uint8_t *payload = (uint8_t *)malloc(payload_len + 1);
        if (payload == NULL) {
            printf("payload malloc failed.\r\n");
            return HI_ERR_FAILURE;
        }
        memset_s(payload, payload_len + 1, 0, payload_len + 1);
        memcpy_s(payload, payload_len + 1, ndefBuff + NDEF_PROTOCOL_VALID_DATA_OFFSET, payload_len);
        printf("Use Wechat system..........\n");
        printf("payload = %s\r\n", payload);

        cJSON *root = cJSON_Parse(payload);
        if (root) {
            cJSON *ssid = cJSON_GetObjectItem(root, "ssid");
            cJSON *password = cJSON_GetObjectItem(root, "passwd");
            if (ssid != NULL && password != NULL) {
                printf("ssid = %s, password = %s", ssid->valuestring, password->valuestring);
                if (0 != strcpy_s(wifi_name, strlen(ssid->valuestring), ssid->valuestring)) {
                    cJSON_Delete(root);
                    free(payload);
                    root = payload = NULL;
                    return HI_ERR_FAILURE;
                }

                if (0 != strcpy_s(wifi_passwd, strlen(password->valuestring), password->valuestring)) {
                    cJSON_Delete(root);
                    free(payload);
                    root = payload = NULL;
                    return HI_ERR_FAILURE;
                }
                ret = 1; // 成功获取到WiFi名称和密码
            }
            ssid = NULL;
            password = NULL;
        }
        cJSON_Delete(root);
        root = NULL;
        free(payload);
        payload = NULL;
    }

    if (ret) {
       	printf("wifi_name: %s\n", wifi_name);
        printf("wifi_passwd: %s\n", wifi_passwd);
        // 连接wifi
        if (WIFI_SUCCESS == WiFi_connectHotspots(wifi_name, wifi_passwd)) {
            printf("thongth to nfc connect wifi is success.\r\n");
            ret = 0;
        } else {
            printf("thongth to nfc connect wifi is failed.\r\n");
            ret = 1;
        }
    }
    return ret;
}
