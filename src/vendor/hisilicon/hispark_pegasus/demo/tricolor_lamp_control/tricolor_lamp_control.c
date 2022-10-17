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
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio_ex.h"
#include "iot_gpio.h"
#include "pca9555.h"

static void TraColorLampControl(void)
{
    // IO扩展芯片初始化
    PCA9555Init();
    // 配置IO扩展芯片的part1的所有管脚为输出
    SetPCA9555GpioValue(PCA9555_PART1_IODIR, PCA9555_OUTPUT);
    // 配置左右三色车灯全灭
    SetPCA9555GpioValue(PCA9555_PART1_OUTPUT, LED_OFF);

    /*
        控制左三色车灯跑马灯，绿、蓝、红、白：每隔一秒一次亮
        绿灯：IO1_3 ==> 0000 1000 ==> 0x08
        蓝灯：IO1_4 ==> 0001 0000 ==> 0x10
        红灯：IO1_5 ==>  0010 0000 ==> 0x20
        白灯：三灯全亮 ==>  0011 1000 ==> 0x38
    */
    while (1) {
        // 设置红灯：IO1_3 输出高电平点亮左车绿灯
        SetPCA9555GpioValue(PCA9555_PART1_OUTPUT, GREEN_LED);
        // 延时函数毫秒（设置高电平持续时间）
        TaskMsleep(DELAY_MS);
        // 设置 绿灯：IO1_4 输出高电平点亮左车蓝灯
        SetPCA9555GpioValue(PCA9555_PART1_OUTPUT, BLUE_LED);
        // 延时函数毫秒（设置低电平持续时间）
        TaskMsleep(DELAY_MS);
        // 蓝灯：IO1_5 输出高电平点亮左车红灯
        SetPCA9555GpioValue(PCA9555_PART1_OUTPUT, RED_LED);
        // 延时函数毫秒（设置高电平持续时间）
        TaskMsleep(DELAY_MS);
        // 设置 IO1_3 IO1_4 IO1_5 都输出高电平，左车亮白灯
        SetPCA9555GpioValue(PCA9555_PART1_OUTPUT, WHITE_LED);
        // 延时函数毫秒（设置低电平持续时间）
        TaskMsleep(DELAY_MS);
    }
}

static void TraColorLampControlEntry(void)
{
    osThreadAttr_t attr;
    attr.name = "LedCntrolDemo";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024; /* 堆栈大小为1024 */
    attr.priority = osPriorityNormal;
    // 报错
    if (osThreadNew((osThreadFunc_t)TraColorLampControl, NULL, &attr) == NULL) {
        printf("[LedExample] Failed to create LedTask!\n");
    }
}

APP_FEATURE_INIT(TraColorLampControlEntry);