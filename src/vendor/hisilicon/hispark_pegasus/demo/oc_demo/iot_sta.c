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

// this demo make the wifi to connect to the specified AP

#include <lwip/ip_addr.h>
#include <lwip/netifapi.h>
#include <hi_types_base.h>
#include <hi_wifi_api.h>
#include "cmsis_os2.h"
#include "iot_config.h"
#include "iot_log.h"
#include "lwip/api_shell.h"
#include "wifi_device.h"

#define APP_INIT_VAP_NUM 2
#define APP_INIT_USR_NUM 2
unsigned char g_wifiStatus = 0;

unsigned char g_wifiFirstConnecting = 0;
unsigned char g_wifiSecondConnecting = 0;
unsigned char g_wifiSecondConnected = 0;
static struct netif* g_iFace = NULL;
void WifiStopSta(int netId);
static int WifiStartSta(void);
int g_netId = -1;
int g_connected = 0;

#define WIFI_CONNECT_STATUS ((unsigned char)0x02)
/* clear netif's ip, gateway and netmask */

static void PrintLinkedInfo(WifiLinkedInfo* info)
{
    if (!info)
        return;

    static char macAddress[32] = { 0 };
    unsigned char* mac = info->bssid;
    if (snprintf_s(macAddress, sizeof(macAddress) + 1, sizeof(macAddress), "%02X:%02X:%02X:%02X:%02X:%02X", mac[0],
                   mac[1], mac[2], mac[3], mac[4], mac[5]) < 0) { /* mac地址从0,1,2,3,4,5位 */
        return;
    }
}

static void OnWifiConnectionChanged(int state, WifiLinkedInfo* info)
{
    if (!info)
        return;

    printf("%s %d, state = %d, info = \r\n", __FUNCTION__, __LINE__, state);
    PrintLinkedInfo(info);

    if (state == WIFI_STATE_AVALIABLE) {
        g_connected = 1;
    } else {
        g_connected = 0;
    }
}

static void OnWifiScanStateChanged(int state, int size)
{
    printf("%s %d, state = %X, size = %d\r\n", __FUNCTION__, __LINE__, state, size);
}

static WifiEvent g_defaultWifiEventListener = { .OnWifiConnectionChanged = OnWifiConnectionChanged,
                                                .OnWifiScanStateChanged = OnWifiScanStateChanged };

static int WifiStartSta(void)
{
    WifiDeviceConfig apConfig = { 0 };
    (void)strcpy_s(apConfig.ssid, strlen(CONFIG_AP_SSID) + 1, CONFIG_AP_SSID);
    (void)strcpy_s(apConfig.preSharedKey, strlen(CONFIG_AP_PWD) + 1, CONFIG_AP_PWD);
    apConfig.securityType = WIFI_SEC_TYPE_PSK;

    WifiErrorCode errCode;
    int netId = -1;

    errCode = RegisterWifiEvent(&g_defaultWifiEventListener);
    printf("RegisterWifiEvent: %d\r\n", errCode);

    errCode = EnableWifi();
    printf("EnableWifi: %d\r\n", errCode);

    errCode = AddDeviceConfig(&apConfig, &netId);
    printf("AddDeviceConfig: %d\r\n", errCode);

    g_connected = 0;
    errCode = ConnectTo(netId);
    printf("ConnectTo(%d): %d\r\n", netId, errCode);

    while (!g_connected) { // wait until connect to AP
        osDelay(10);       /* 等待1000ms */
    }
    printf("g_connected: %d\r\n", g_connected);

    g_iFace = netifapi_netif_find("wlan0");
    if (g_iFace) {
        err_t ret = netifapi_dhcp_start(g_iFace);
        printf("netifapi_dhcp_start: %d\r\n", ret);

        osDelay(100); // wait 100ms DHCP server give me IP
        ret = netifapi_netif_common(g_iFace, dhcp_clients_info_show, NULL);
        printf("netifapi_netif_common: %d\r\n", ret);
    }
    return netId;
}

void WifiStopSta(int netId)
{
    if (g_iFace) {
        err_t ret = netifapi_dhcp_stop(g_iFace);
        printf("netifapi_dhcp_stop: %d\r\n", ret);
    }

    WifiErrorCode errCode = Disconnect(); // disconnect with your AP
    printf("Disconnect: %d\r\n", errCode);

    errCode = UnRegisterWifiEvent(&g_defaultWifiEventListener);
    printf("UnRegisterWifiEvent: %d\r\n", errCode);

    RemoveDevice(netId); // remove AP config
    printf("RemoveDevice: %d\r\n", errCode);

    errCode = DisableWifi();
    printf("DisableWifi: %d\r\n", errCode);
}

void WifiStaReadyWait(void)
{
    ip4_addr_t ipAddr;
    ip4_addr_t ipAny;
    IP4_ADDR(&ipAny, 0, 0, 0, 0);
    IP4_ADDR(&ipAddr, 0, 0, 0, 0);
    g_netId = WifiStartSta();
    printf("wifi sta dhcp done");
    return;
}