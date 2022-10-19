#include <stdio.h>
#include <unistd.h>

#include "cw2015.h"
#include "pca9555.h"
#include "iot_gpio_ex.h"
#include "iot_i2c.h"
#include "iot_gpio.h"
#include "iot_errno.h"
#include "hi_errno.h"
#include "hi_i2c.h"

uint32_t CW2015WriteRead(uint8_t reg_high_8bit_cmd, uint8_t send_len, uint8_t read_len)
{
    uint32_t status =0;
    uint32_t ret = 0;
    uint8_t recvData[888] = {0};
    hi_i2c_data i2c_write_cmd_addr ={0};
    uint8_t send_user_cmd[1] = {reg_high_8bit_cmd};
    memset(recvData, 0x0, sizeof(recvData));
    i2c_write_cmd_addr.send_buf = send_user_cmd;
    i2c_write_cmd_addr.send_len = send_len;

    i2c_write_cmd_addr.receive_buf = recvData;
    i2c_write_cmd_addr.receive_len = read_len;

    status = hi_i2c_writeread(PCA9555_I2C_IDX, CW2015_READ_ADDR, &i2c_write_cmd_addr);
    if (status != HI_ERR_SUCCESS) {
        printf("I2cRead() failed, %0X!\n", status);
        return status;
    }
    ret = recvData[0];
    return ret;
}

uint32_t CW2015Write(uint8_t addr, uint8_t writedata, uint32_t buffLen)
{
    uint8_t buffer[2] = {addr, writedata};
    uint32_t retval = IoTI2cWrite(PCA9555_I2C_IDX, CW2015_WRITE_ADDR, buffer, buffLen);
    if (retval != IOT_SUCCESS) {
        printf("IoTI2cWrite(%02X) failed, %0X!\n", buffer[0], retval);
        return retval;
    }

    return IOT_SUCCESS;
}

void CW2015Init()
{
    // 初始化 PCA9555 IO扩展模块
    // PCA9555Init();
    // 使电量检测模块从sleep mode变为wake up mode
    CW2015Write(0x0A, 0x00, 2);
}

float GetVoltage(void)
{
    uint8_t buff[2] = {0};
    float voltage = 0;
    uint32_t temp = 0;
    // 读取电压的前6位
    buff[0] = CW2015WriteRead(0x02, 1, 1);
    // 读取电压的后8位
    buff[1] = CW2015WriteRead(0x03, 1, 1);
    // 通过位运算最后得到14位的A/D测量值
    temp = (buff[0] << 8) | buff[1];
    // 通过计算得到最终的电压值 （CW2015的电压分辨率为305uV）
    voltage = temp * 305.0 / 1000000;

    return voltage;
}