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
/**
 * @brief  使用NFC进行配网
 * @note   驱动NDEF协议中的第一个标签数据，然后进行配网，
 * @param  *ndefBuff: 标签数据的缓冲区
 * @retval
 */
uint32_t NFC_configuresWiFiNetwork(uint8_t *ndefBuff)
{
    if (ndefBuff == NULL) {
        printf("NFC_configuresWiFiNetwork to ndefBuff is NULL\r\n");
        return 1;
    }
    uint8_t ret = 0;
    uint8_t payloadLength = ndefBuff[NDEF_PROTOCOL_DATA_LENGTH_OFFSET]; // 获取数据长度
    printf("payload Length is %d.\r\n", payloadLength);

    if (ndefBuff[NDEF_PROTOCOL_DATA_TYPE_OFFSET] == 't') {
        // 查看是不是文本信息
        uint8_t *payload = (uint8_t *)malloc(payloadLength + 1);
        if (payload == NULL) {
            printf("payload malloc failed.\r\n");
            return 1;
        }

        memset_s(payload, payloadLength + 1, 0, payloadLength);
        memcpy_s(payload, payloadLength + 1, ndefBuff + NDEF_PROTOCOL_VALID_DATA_OFFSET, payloadLength);
        payload[payloadLength] = '\0';

        printf("payload = %s\r\n", payload);
        cJSON *root = cJSON_Parse(payload);
        if (root) {
            cJSON *ssid = cJSON_GetObjectItem(root, "ssid");
            cJSON *password = cJSON_GetObjectItem(root, "passwd");
            if (ssid != NULL && password != NULL) {
                printf("ssid = %s, password = %s\r\n", ssid->valuestring, password->valuestring);
                // 连接wifi
                if (WIFI_SUCCESS == WiFi_connectHotspots(ssid->valuestring, password->valuestring)) {
                    printf("thongth to nfc connect wifi is success.\r\n");
                    ret = 0;
                } else {
                    printf("thongth to nfc connect wifi is failed.\r\n");
                    ret = 1;
                }
            }
            ssid = NULL;
            password = NULL;
        }
        cJSON_Delete(root);
        root = NULL;

        free(payload);
        payload = NULL;
    } else {
        printf("data type is not 't'!\r\n");
        return 1;
    }

    return ret;
}
