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
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio_ex.h"
#include "iot_i2c.h"
#include "iot_gpio.h"
#include "iot_errno.h"
#include "iot_watchdog.h"
#include "hi_errno.h"
#include "hi_i2c.h"
#include "oled_ssd1306.h"

#define IOT_I2C_IDX_BAUDRATE (400 * 1000)
#define CW2015_I2C_IDX 0
#define IOT_PWM_PORT_PWM3   3
#define CW2015_READ_ADDR     (0xC5)
#define CW2015_WRITE_ADDR    (0xC4)
#define WRITELEN  2
#define CW2015_HIGHT_REGISTER 0x02
#define CW2015_LOW_REGISTER   0x03
#define CW2015_WAKE_REGISTER  0x0A
#define DELYA_US20            20

/**
 * @berf i2c read
 * @param hi_u8 reg_high_8bit_cmd:Transmit register value 8 bits high
 * @param hi_u8 reg_low_8bit_cmd:Transmit register value low 8 bits
 * @param hi_u8* recv_data:Receive data buff
 * @param hi_u8 send_len:Sending data length
 * @param hi_u8 read_len:Length of received data
*/
uint32_t Cw20_WriteRead(uint8_t reg_high_8bit_cmd, uint8_t send_len, uint8_t read_len)
{
    uint32_t status = 0;
    uint32_t ret = 0;
    uint8_t recvData[888] = { 0 };
    hi_i2c_data i2c_write_cmd_addr = { 0 };
    uint8_t send_user_cmd[1] = {reg_high_8bit_cmd};
    memset(recvData, 0x0, sizeof(recvData));
    i2c_write_cmd_addr.send_buf = send_user_cmd;
    i2c_write_cmd_addr.send_len = send_len;

    i2c_write_cmd_addr.receive_buf = recvData;
    i2c_write_cmd_addr.receive_len = read_len;

    status = hi_i2c_writeread(CW2015_I2C_IDX, CW2015_READ_ADDR, &i2c_write_cmd_addr);
    if (status != IOT_SUCCESS) {
        printf("I2cRead() failed, %0X!\n", status);
        return status;
    }
    ret = recvData[0];
    return ret;
}

uint32_t Cw20_Write(uint8_t addr, uint8_t writedata, uint32_t buffLen)
{
    uint8_t buffer[2] = {addr, writedata};
    uint32_t retval = IoTI2cWrite(CW2015_I2C_IDX, CW2015_WRITE_ADDR, buffer, buffLen);
    if (retval != IOT_SUCCESS) {
        printf("IoTI2cWrite(%02X) failed, %0X!\n", buffer[0], retval);
        return retval;
    }
    printf("IoTI2cWrite(%02X)\r\n", buffer[0]);
    return IOT_SUCCESS;
}

void CW2015Init(void)
{
    // 初始化I2C设备0，并指定波特率为400k
    IoTI2cInit(CW2015_I2C_IDX, IOT_I2C_IDX_BAUDRATE);
    // 设置I2C设备0的波特率为400k
    IoTI2cSetBaudrate(CW2015_I2C_IDX, IOT_I2C_IDX_BAUDRATE);
    // 设置GPIO13的管脚复用关系为I2C0_SDA
    IoSetFunc(IOT_IO_NAME_GPIO_13, IOT_IO_FUNC_GPIO_13_I2C0_SDA);
    // 设置GPIO14的管脚复用关系为I2C0_SCL
    IoSetFunc(IOT_IO_NAME_GPIO_14, IOT_IO_FUNC_GPIO_14_I2C0_SCL);
    // 使电量检测模块从sleep mode变为wake up mode,0x00代表唤醒,0x11代表沉睡,2bit控制
    Cw20_Write(CW2015_WAKE_REGISTER, 0x00, 2);
}

float GetVoltage(void)
{
    uint8_t buff[WRITELEN] = {0};
    float voltage = 0;
    uint32_t temp = 0;
    // 读取电压的前6位
    buff[0] = Cw20_WriteRead(CW2015_HIGHT_REGISTER, 1, 1);
    // 读取电压的后8位
    buff[1] = Cw20_WriteRead(CW2015_LOW_REGISTER, 1, 1);
    // 通过位运算最后得到14位的A/D测量值
    temp = (buff[0] << 8) | buff[1]; /* 将buf[0]左移8位与buf[1]组成最终电压值 */
    // 通过计算得到最终的电压值 （CW2015的电压分辨率为305.0uV,转换1uv = 1 / 1000000）
    voltage = temp * 305.0 / 1000000;
    return voltage;
}

static void CW2015Task(void)
{
    static char line[32] = {0};
    float voltage = 0.0;

    // I2C0初始化
    CW2015Init();
    // OLED初始化
    OledInit();
    OledFillScreen(0);
    while (1) {
        voltage = GetVoltage();
        // 将获取到的电源格式化为字符串
        int ret = snprintf(line, sizeof(line), "voltage: %.2f", voltage);
        if (ret != 13) { // 需要显示的字符串长度为13
            printf("GetVoltage failed\r\n");
        }
         /* 在OLED屏幕的第20列5行显示1行 */
        OledShowString(20, 5, line, 1);
        usleep(DELYA_US20);
    }
}

void CW2015SampleEntry(void)
{
    osThreadAttr_t attr;
    IoTWatchDogDisable();
    attr.name = "CW2015Task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 5; /* 堆栈大小为1024*5 */
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)CW2015Task, NULL, &attr) == NULL) {
        printf("[CW2015Task] Failed to create CW2015TaskSampleTask!\n");
    }
}

APP_FEATURE_INIT(CW2015SampleEntry);