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
#include <stdlib.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "hi_io.h"
#include "ssd1306.h"
#include "iot_errno.h"
#include "iot_gpio_ex.h"
#include "pca9555.h"
#include "motor_control.h"
#include "hi_time.h"
#include "trace_control.h"

CAR_DRIVE car_drive = { 0 };
ENUM_MODE g_mode = MODE_ON_OFF;

int g_CarStarted = 0;

#define MASK_BUTTON1        (0x10)
#define MASK_BUTTON2        (0x08)
#define MASK_BUTTON3        (0x04)
#define MASK_TRACE1         (0x20)
#define MASK_TRACE2         (0x40)

void init_ctrl_algo(void)
{
    (void)memset(car_drive, 0, sizeof(CAR_DRIVE));
    car_drive.LeftForward = 10; // 10 左轮前进速度
    car_drive.RightForward = 10; // 10 右轮前进速度
    car_drive.TurnLeft = 30; // 30 左转弯右轮速度
    car_drive.TurnRight = 30; // 30 右转弯左轮速度
}

void init_oled_mode(void)
{
    g_mode = MODE_ON_OFF;
    ssd1306_ClearOLED();
    ssd1306_printf("LF:%d, RF:%d", car_drive.LeftForward, car_drive.RightForward);
    ssd1306_printf("TL:%d, RT:%d", car_drive.TurnLeft, car_drive.TurnRight);
}

void ButtonDesplay(ENUM_MODE mode)
{
    switch (mode) {
        case MODE_ON_OFF:
            ssd1306_printf("LF:%d, RF:%d", car_drive.LeftForward, car_drive.RightForward);
            ssd1306_printf("TL:%d, TR:%d", car_drive.TurnLeft, car_drive.TurnRight);
            break;
        case MODE_SET_LEFT_FORWARD:
            ssd1306_printf("Set LForward=%d", car_drive.LeftForward);
            break;
        case MODE_SET_RIGHT_FORWARD:
            ssd1306_printf("Set RForward=%d", car_drive.RightForward);
            break;
        case MODE_SET_TURN_LEFT:
            ssd1306_printf("Set TurnLeft=%d", car_drive.TurnRight);
            break;
        case MODE_SET_TURN_RIGHT:
            ssd1306_printf("Set TurnRight=%d", car_drive.TurnLeft);
            break;
        default:
            break;
    }
}

void ButtonSet(ENUM_MODE mode, bool button_pressed)
{
    printf("mode = %d\r\n", mode);
    switch (mode) {
        case MODE_ON_OFF:
            g_CarStarted = !g_CarStarted;
            ssd1306_ClearOLED();
            printf("g_CarStarted = %d\r\n", g_CarStarted);
            ssd1306_printf(g_CarStarted ? "start" : "stop");
            break;
        case MODE_SET_LEFT_FORWARD:
            car_drive.LeftForward += ((button_pressed) ? -1 : 1);
            ssd1306_printf("LeftForward=%d", car_drive.LeftForward);
            break;
        case MODE_SET_RIGHT_FORWARD:
            car_drive.RightForward += (button_pressed ? -1 : 1);
            ssd1306_printf("RightForward=%d", car_drive.RightForward);
            break;
        case MODE_SET_TURN_LEFT:
            car_drive.TurnLeft += ((button_pressed) ? -1 : 1);
            ssd1306_printf("TurnLeft=%d", car_drive.TurnRight);
            break;
        case MODE_SET_TURN_RIGHT:
            car_drive.TurnRight += ((button_pressed) ? -1 : 1);
            ssd1306_printf("TurnRight=%d", car_drive.TurnLeft);
            break;
        default:
            break;
    }
}

void ButtonPressProc(uint8_t ext_io_val)
{
    static uint8_t ext_io_val_d = 0xFF;
    uint8_t diff;
    static int time_stamp = 0;
    bool button1_pressed, button2_pressed, button3_pressed, trace1_pressed, trace2_pressed;
    diff = ext_io_val ^ ext_io_val_d;
    button1_pressed = ((diff & MASK_BUTTON1) && ((ext_io_val & MASK_BUTTON1) == 0)) ? true : false;
    button2_pressed = ((diff & MASK_BUTTON2) && ((ext_io_val & MASK_BUTTON2) == 0)) ? true : false;
    button3_pressed = ((diff & MASK_BUTTON3) && ((ext_io_val & MASK_BUTTON3) == 0)) ? true : false;
    trace1_pressed = ((diff & MASK_TRACE1) && ((ext_io_val & MASK_TRACE1) == 0)) ? true : false;
    trace2_pressed = ((diff & MASK_TRACE2) && ((ext_io_val & MASK_TRACE2) == 0)) ? true : false;
    ssd1306_ClearOLED();
    if (button1_pressed) {
        g_mode = (g_mode >= (MODE_END - 1)) ? 0 : (g_mode + 1);
        ButtonDesplay(g_mode);
    } else if (button2_pressed || button3_pressed) {
        ButtonSet(g_mode, button2_pressed);
    }
    if((hi_get_milli_seconds() - time_stamp) > 100 && g_CarStarted) {
        time_stamp = hi_get_milli_seconds();
        if (trace1_pressed && !trace2_pressed) {
            car_left(car_drive.TurnRight);
            printf("11111111111111\r\n");
        } else if (!trace1_pressed & trace2_pressed) {
            car_right(!car_drive.TurnLeft);
            printf("2222222222222222\r\n");
        } else if(!trace2_pressed && !trace2_pressed) {
            car_forward(car_drive.LeftForward, car_drive.RightForward);
            printf("333333333333333\r\n");
        } else {
            car_stop();
        }
    } else if (!g_CarStarted) {
        car_stop();
    }
    ext_io_val_d = ext_io_val;
}

void TraceDemoTask(void)
{
    InitPCA9555();
    GA12N20Init();
    TaskMsleep(100); // 等待100ms初始化完成
    init_ctrl_algo();
    init_oled_mode();
    PCA_RegisterEventProcFunc(ButtonPressProc);
}

void TraceSampleEntry(void)
{
    osThreadAttr_t attr;
    attr.name = "TraceSampleEntry";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 5; /* 堆栈大小为1024*5 */
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)TraceDemoTask, NULL, &attr) == NULL) {
        printf("[TraceSampleEntry] Failed to create TraceDemoTask!\n");
    }
}
APP_FEATURE_INIT(TraceSampleEntry);