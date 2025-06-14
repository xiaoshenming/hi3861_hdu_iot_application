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

#include <stdio.h>
#include <string.h>
#include "ohos_init.h"
#include "cmsis_os2.h"

#include "wifi_connecter.h"

#include "config.h"
static void WifiConnectTask(void)
{
    osDelay(10); /* 10 = 100ms */
    // setup your AP params
    WifiDeviceConfig apConfig = {0};
    strcpy(apConfig.ssid, WIFI_SSID);             // 设置wifi ssid "h" Set wifi ssid
    strcpy(apConfig.preSharedKey, WIFI_PASSWORD); // 设置wifi 密码 "12345678" Set wifi password
    apConfig.securityType = WIFI_SEC_TYPE_PSK;

    int netId = ConnectToHotspot(&apConfig);
    printf("success to setup AP params\n");
    osDelay(100);
    // DisconnectWithHotspot(netId);
}

static void WifiConnectDemo(void)
{
    osThreadAttr_t attr;
    printf("my WIFI test\n");
    osDelay(100);
    attr.name = "WifiConnectTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 10240; // 任务栈10240, stack size 10240
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)WifiConnectTask, NULL, &attr) == NULL)
    {
        printf("[WifiConnectDemo] Falied to create WifiConnectTask!\n");
    }
}

SYS_RUN(WifiConnectDemo);
