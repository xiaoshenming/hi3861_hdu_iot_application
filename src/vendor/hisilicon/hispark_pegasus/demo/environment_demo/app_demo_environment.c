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

#include <iot_pwm.h>
#include <stdio.h>
#include <unistd.h>
#include "app_demo_aht20.h"
#include "app_demo_config.h"
#include "app_demo_i2c_oled.h"
#include "app_demo_mq2.h"
#include "app_demo_multi_sample.h"
#include "cmsis_os2.h"
#include "iot_i2c.h"
#include "ohos_init.h"
#include "app_demo_environment.h"

/* temperature menu display */
void ShowTemperatureValue(void)
{
    static unsigned short currentMode = 0;
    static char* line = 0;
    IoTI2cInit(0, HI_I2C_IDX_BAUDRATE); /* baudrate: 400000 */
    IoTI2cSetBaudrate(0, HI_I2C_IDX_BAUDRATE);
    currentMode = GetKeyStatus(CURRENT_MODE);
    while (1) {
        hi_udelay(DELAY_10_MS); // delay 10ms
        GetAht20SensorData();
        snprintf(line, sizeof(line), "%.2f", GetAhtSensorValue(AHT_TEMPERATURE));
        OledShowStr(40, 5, line, 1); /* 40, 5, x.xx, 1 */
        if (currentMode != GetKeyStatus(CURRENT_MODE)) {
            currentMode = GetKeyStatus(CURRENT_MODE);
            break;
        }
        TaskMsleep(SLEEP_10_MS); // 10ms
    }
}

/* humidity value display */
void ShowHumidityValue(void)
{
    static unsigned short currentMode = 0;
    static char line[32] = {0};
    IoTI2cInit(0, HI_I2C_IDX_BAUDRATE); /* baudrate: 400000 */
    IoTI2cSetBaudrate(0, HI_I2C_IDX_BAUDRATE);
    currentMode = GetKeyStatus(CURRENT_MODE);
    while (1) {
        hi_udelay(DELAY_10_MS); // delay 10ms
        GetAht20SensorData();
        snprintf(line, sizeof(line), "%.2f", GetAhtSensorValue(AHT_HUMIDITY));
        OledShowStr(OLED_X_POSITION_56, OLED_Y_POSITION_5, line,
                    OLED_DISPLAY_STRING_TYPE_1); /* 56, 5, x.xx, 1 */
        if (currentMode != GetKeyStatus(CURRENT_MODE)) {
            break;
        }
        TaskMsleep(SLEEP_10_MS); // 10ms
    }
}

/* combustible gas value display */
void ShowCombustibleGasValue(void)
{
    unsigned short currentMode = 0;
    static char line[32] = {0};
    IoTI2cInit(0, HI_I2C_IDX_BAUDRATE); /* baudrate: 400000 */
    IoTI2cSetBaudrate(0, HI_I2C_IDX_BAUDRATE);
    currentMode = GetKeyStatus(CURRENT_MODE);
    while (1) {
        SetCombuSensorValue();
        Mq2GetData();
        snprintf(line, sizeof(line), "%.2f", GetCombuSensorValue());
        if (!GetCombuSensorValue()) {
            OledShowStr(OLED_X_POSITION_60, OLED_Y_POSITION_5, "0.00    ",
                        OLED_DISPLAY_STRING_TYPE_1); /* 60, 5, x.xx, 1 */
        } else {
            OledShowStr(OLED_X_POSITION_60, OLED_Y_POSITION_5, line,
                        OLED_DISPLAY_STRING_TYPE_1); /* 60, 5, x.xx, 1 */
        }
        if (currentMode != GetKeyStatus(CURRENT_MODE)) {
            currentMode = GetKeyStatus(CURRENT_MODE);
            break;
        }
        TaskMsleep(SLEEP_10_MS); // 10ms
    }
}

/* environment function hamdle and display */
void EnvironmentFunc(void)
{
    /* i2c baudrate setting */
    IoTI2cInit(0, HI_I2C_IDX_BAUDRATE);        /* baudrate: 400kbps */
    IoTI2cSetBaudrate(0, HI_I2C_IDX_BAUDRATE); /* 0, 400kbps */
    /* init oled i2c */
    IoTGpioInit(HI_GPIO_13);               /* GPIO13 */
    IoSetFunc(HI_GPIO_13, HI_I2C_SDA_SCL); /* GPIO13,  SDA */
    IoTGpioInit(HI_GPIO_14);               /* GPIO 14 */
    IoSetFunc(HI_GPIO_14, HI_I2C_SDA_SCL); /* GPIO14  SCL */
    EnvironmentDemoDisplay();
}