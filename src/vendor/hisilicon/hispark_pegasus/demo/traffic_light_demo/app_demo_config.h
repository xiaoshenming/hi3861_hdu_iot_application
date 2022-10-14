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

#ifndef APP_DEMO_CONFIG_H
#define APP_DEMO_CONFIG_H

#define OLED_DISPLAY_STRING_TYPE_1  (1)
#define OLED_DISPLAY_STRING_TYPE_16 (16)

typedef enum {
    TIME_COUNT_0 = 0,
    TIME_COUNT_1,
    TIME_COUNT_2,
    TIME_COUNT_3,
    TIME_COUNT_4,
    TIME_COUNT_5,
    TIME_COUNT_6
} TimeCountEnv;

typedef struct LightTimer {
    int Timer1Status;
    int Timer1Count;
    int Timer2Status;
    int Timer2Count;
    int Timer3Status;
    int Timer3Count;
    int Timer4Status;
    int Timer4Count;
    int Timer5Status;
    int Timer5Count;
    int Timer6Status;
    int Timer6Count;
    int Timer7Status;
    int Timer7Count;
    int Timer8Status;
    int Timer8Count;
    int Timer9Status;
    int Timer9Count;
    int Timer10Status;
    int Timer10Count;
    int TimerRedLightStatus;
    int TimerYellowLightStatus;
    int TimerGreenLightStatus;
    int HumanTimerRedLightStatus;
    int HumanTimerYellowLightStatus;
    int HumanTimerGreenLightStatus;
} LightTimerCfg;

typedef struct {
    int timer1Count;
    int timer2Count;
} TimerMsg;

typedef struct {
    unsigned int g_redLedAutoModuTimeCount;
    unsigned int g_yellowLedAutoModuTimeCount;
    unsigned int g_greenLedAutoModuTimeCount;
    unsigned int g_redLedHumanModuTimeCount;
    unsigned int g_yellowLedHumanModuTimeCount;
    unsigned int g_greenLedHumanModuTimeCount;
} TrafficLedStatusType;

unsigned char DelayAndFreshScreen(unsigned int delayTime, unsigned char beepStatus);
void TrafficAutoModeSample(void);
void TrafficNormalType(void);
void TrafficHumanType(void);
void TrafficHumanModeSample(void);
void SoftwareTimersTaskEntry(void);
unsigned char SetTimeCount(unsigned char setTimeCount);
void TrafficDisplay(void);
#endif