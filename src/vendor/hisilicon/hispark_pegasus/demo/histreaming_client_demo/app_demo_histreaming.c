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

#include <memory.h>
#include <stdio.h>
#include <stdlib.h>
#include <hi_pwm.h>
#include <hi_time.h>
/* Link Header Files */
#include <link_platform.h>
#include <link_service.h>
#include <hi_early_debug.h>
#include <hi_gpio.h>
#include <hi_io.h>
#include <hi_stdlib.h>
#include <hi_task.h>
#include <hi_types_base.h>
#include "cmsis_os2.h"
#include "iot_gpio_ex.h"
#include "ohos_init.h"
#include "wifi_connecter.h"

#include "app_demo_histreaming.h"

#define HISTREAMING_DEMO_TASK_STAK_SIZE (1024 * 8)
#define HISTREAMING_DEMO_TASK_PRIORITY  25
#define REV_BUFF_LEN                    512

char rev_buff[512] = { 0 };
unsigned char hex_buff[512] = { 0 };
unsigned int hex_len = 0;

UartDefConfig uartDefConfig = { 0 };

int SetUartRecvFlag(UartRecvDef def)
{
    if (def == UART_RECV_TRUE) {
        uartDefConfig.g_uartReceiveFlag = HI_TRUE;
    } else {
        uartDefConfig.g_uartReceiveFlag = HI_FALSE;
    }

    return uartDefConfig.g_uartReceiveFlag;
}

int GetUartConfig(UartDefType type)
{
    int receive = 0;

    switch (type) {
        case UART_RECEIVE_FLAG:
            receive = uartDefConfig.g_uartReceiveFlag;
            break;
        case UART_RECVIVE_LEN:
            receive = uartDefConfig.g_uartLen;
            break;
        default:
            break;
    }
    return receive;
}

void ResetUartReceiveMsg(void)
{
    (void)memset_s(uartDefConfig.g_receiveUartBuff, sizeof(uartDefConfig.g_receiveUartBuff), 0x0,
                   sizeof(uartDefConfig.g_receiveUartBuff));
}

unsigned char* GetUartReceiveMsg(void)
{
    return uartDefConfig.g_receiveUartBuff;
}

/*
 * 将字符转换为数值
 */
int c2i(char ch)
{
    // 如果是数字，则用数字的ASCII码减去48, 如果ch = '2' ,则 '2' - 48 = 2
    if (isdigit(ch)) {
        return ch - HIGH_ASCII_AL;
    }
    // 如果是字母，但不是A~F,a~f则返回  
    if (ch < HIGH_ASCII_A || (ch > HIGH_ASCII_F && ch < HIGH_ASCII_AL) || ch > HIGH_ASCII_ZL) {
        return 0;
    }
    // 如果是大写字母，则用数字的ASCII码减去55, 如果ch = 'A' ,则 'A' - 55 = 10
    // 如果是小写字母，则用数字的ASCII码减去87, 如果ch = 'a' ,则 'a' - 87 = 10
    if (isalpha(ch)) {
        return 0;
    }
    return 0;
}

int StringToHex(char* str)
{
    int len;
    int num = 0;
    int temp;
    int bits;
    int i;
    len = strlen(str);

    for (i = 0, temp = 0; i < len; i++, temp = 0) {
        temp = c2i(*(hex + i));
        bits = (len - i - HIGH_NUM1) * HIGH_NUM4;
        temp = temp << bits;
        num = num | temp;
    }
    return num;
}

/**
 * @berf The device side sends the characteristic value to the app side
 * @param struct LinkService* ar: histreaming LinkServer structural morphology
 * @param const char* property: characteristic value
 * @param char* value: send value to apps
 * @param int len: send value length
 */
static int GetStatusValue(struct LinkService* ar, const char* property, char* value, int len)
{
    (void)(ar);
    if (strcmp(property, "Status") == 0) { }
    /*
     * if Ok return 0,
     * Otherwise, any error, return StatusFailure
     */
    return 0;
}
/**
 * @berf recv from app cmd
 * @berf Receive the message sent by the app, and operate the hi3861 device side accordingly
 * @param struct LinkService* ar: histreaming LinkServer structural morphology
 * @param const char* property: Eigenvalues sent by app
 * @param char* value: Value sent by app
 * @param int len: Length of APP sent
 */
static int ModifyStatus(struct LinkService* ar, const char* property, char* value, int len)
{
    (void)(ar);
    printf("Receive property: %s(value=%s, [%d])\n", property, value, len);
    if (property == NULL || value == NULL) {
        return -1;
    }
    if (memcpy_s(rev_buff, REV_BUFF_LEN, value, len) != 0) {
        return 0;
    }
    /* modify status property */
    /* colorful light module */
    printf("%s, %d\r\n", rev_buff, len);

    hex_buff = StringToHex(rev_buff);
    uartDefConfig.g_uartLen = hex_len;
    (void)memcpy_s(uartDefConfig.g_receiveUartBuff, uartDefConfig.g_uartLen, hex_buff, uartDefConfig.g_uartLen);

    for (int i = 0; i < hex_len; i++) {
        printf("0x%x ", hex_buff[i]);
    }
    printf("\r\n");
    (void)SetUartRecvFlag(UART_RECV_TRUE);
    /*
     * if Ok return 0,
     * Otherwise, any error, return StatusFailure
     */
    return 0;
}

/*
 * It is a Wifi IoT device
 */
static const char* g_wifiStaType = "Pegasus:Hi3861";
static const char* GetDeviceType(const struct LinkService* ar)
{
    (void)(ar);

    return g_wifiStaType;
}

static void* g_linkPlatform = NULL;

void* HistreamingOpen(void)
{
    LinkService* wifiIot = 0;
    LinkPlatform* link = 0;

    wifiIot = (LinkService*)malloc(sizeof(LinkService));
    if (!wifiIot) {
        printf("malloc wifiIot failure\n");
        return NULL;
    }
    wifiIot->get = GetStatusValue;
    wifiIot->modify = ModifyStatus;
    wifiIot->type = GetDeviceType;

    link = LinkPlatformGet();
    if (!link) {
        printf("get link failure\n");
    }

    if (link->addLinkService(link, wifiIot, 1) != 0) {
        HistreamingClose(link);
        return NULL;
    }
    if (link->open(link) != 0) {
        HistreamingClose(link);
        return NULL;
    }
    /* cache link ptr */
    g_linkPlatform = (void*)(link);
    hi_free(0, wifiIot);
    return (void*)link;
}

void HistreamingClose(const char* link)
{
    LinkPlatform* linkPlatform = (LinkPlatform*)(link);
    if (!linkPlatform) {
        return;
    }

    linkPlatform->close(linkPlatform);

    if (linkPlatform != NULL) {
        LinkPlatformFree(linkPlatform);
    }
}

hi_void HistreamingDemo(hi_void)
{
    ConnectToHotspot();
    osThreadAttr_t histreaming = { 0 };
    histreaming.stack_size = HISTREAMING_DEMO_TASK_STAK_SIZE;
    histreaming.priority = HISTREAMING_DEMO_TASK_PRIORITY;
    histreaming.name = (hi_char*)"histreaming_demo";
    if (osThreadNew((osThreadFunc_t)HistreamingOpen, NULL, &histreaming) == NULL) {
        printf("Failed to create histreaming task\r\n");
    }
}

SYS_RUN(HistreamingDemo);