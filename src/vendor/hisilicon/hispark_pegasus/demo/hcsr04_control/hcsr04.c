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

/*
    HCSR05 超声波模块的相关API接口
*/
#include <stdio.h>
#include <stdlib.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_watchdog.h"
#include "iot_gpio.h"
#include "hi_time.h"
#include "hi_io.h"
#include "iot_gpio_ex.h"

#define DELAY_US20    20
#define DELAY_MS10    10

void Hcsr04Init(void)
{
    // 设置超声波Echo为输入模式
    // 设置GPIO8功能（设置为GPIO功能）
    IoSetFunc(IOT_IO_NAME_GPIO_8, IOT_IO_FUNC_GPIO_8_GPIO);
    // 设置GPIO8为输入方向
    IoTGpioSetDir(IOT_IO_NAME_GPIO_8, IOT_GPIO_DIR_IN);

    // 设置GPIO7功能（设置为GPIO功能）
    IoSetFunc(IOT_IO_NAME_GPIO_7, IOT_IO_FUNC_GPIO_7_GPIO);
    // 设置GPIO7为输出方向
    IoTGpioSetDir(IOT_IO_NAME_GPIO_7, IOT_GPIO_DIR_OUT);
}

float GetDistance(void)
{
    // 定义变量
    static unsigned long start_time = 0, time = 0;
    float distance = 0.0;
    IotGpioValue value = IOT_GPIO_VALUE0;
    unsigned int flag = 0;

    // 设置GPIO7输出低电平
    /* 给trig发送至少10us的高电平脉冲，以触发传感器测距 */
    IoTGpioSetOutputVal(IOT_IO_NAME_GPIO_7, IOT_GPIO_VALUE1);
    // 延时函数（设置高电平持续时间）
    hi_udelay(DELAY_US20);
    // 设置GPIO7输出高电平
    IoTGpioSetOutputVal(IOT_IO_NAME_GPIO_7, IOT_GPIO_VALUE0);
    /* 计算与障碍物之间的距离 */
    while (1) {
        // 获取GPIO8的输入电平状态
        IoTGpioGetInputVal(IOT_IO_NAME_GPIO_8, &value);
        // 判断GPIO8的输入电平是否为高电平并且flag为0
        if (value == IOT_GPIO_VALUE1 && flag == 0) {
            // 获取系统时间
            start_time = hi_get_us();
            // 将flag设置为1
            flag = 1;
        }
        // 判断GPIO8的输入电平是否为低电平并且flag为1
        if (value == IOT_GPIO_VALUE0 && flag == 1) {
            // 获取高电平持续时间
            time = hi_get_us() - start_time;
            break;
        }
    }
    // 计算距离障碍物距离（340米/秒 转换为 0.034厘米/微秒, 2代表去来，两倍距离）
    distance = time * 0.034 / 2;
    printf("distance is %0.2f cm\r\n", distance);
    return distance;
}

void Hcsr04SampleTask(void)
{
    printf("Hcsr04SampleTask init\r\n");
    while (1) {
        GetDistance();
        osDelay(DELAY_MS10);
    }
}

void Hcsr04SampleEntry(void)
{
    // 初始化超声波传感器
    osThreadAttr_t attr;
    Hcsr04Init();
    IoTWatchDogDisable();
    attr.name = "Hcsr04SampleTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 5; /* 堆栈大小为1024*5 */
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)Hcsr04SampleTask, NULL, &attr) == NULL) {
        printf("[Hcsr04SampleTask] Failed to create Hcsr04SampleTask!\n");
    }
}
APP_FEATURE_INIT(Hcsr04SampleEntry);