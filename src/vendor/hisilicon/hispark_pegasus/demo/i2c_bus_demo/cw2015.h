/*
    CW2015CTCC 电量检测模块的相关API接口
*/
#ifndef __CW2015_H__
#define __CW2015_H__

#include <stdint.h>

#define IOT_PWM_PORT_PWM3   3
#define CW2015_READ_ADDR     (0XC5)
#define CW2015_WRITE_ADDR    (0XC4)
#define WRITELEN  2

// 往CW2015 I2C设备中发送与接收数据
/**
 * @berf i2c read
 * @param hi_u8 reg_high_8bit_cmd:Transmit register value 8 bits high
 * @param hi_u8 reg_low_8bit_cmd:Transmit register value low 8 bits
 * @param hi_u8* recv_data:Receive data buff
 * @param hi_u8 send_len:Sending data length
 * @param hi_u8 read_len:Length of received data
*/
uint32_t CW2015WriteRead(uint8_t reg_high_8bit_cmd, uint8_t send_len, uint8_t read_len);

// 往CW2015 I2C设备中发送数据
uint32_t CW2015Write(uint8_t addr, uint8_t writedata, uint32_t buffLen);

// 初始化CW2015CTCC 电量检测模块
void CW2015Init();

// 获取电压值
float GetVoltage(void);

#endif // __FUNCKEY_H__