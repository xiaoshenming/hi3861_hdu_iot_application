/*
    SG92R舵机的相关API接口
*/

#include <stdio.h>
#include <stdlib.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "hi_io.h"
#include "iot_gpio_ex.h"
#include "hi_time.h"

#define  COUNT   10

void SetAngle(unsigned int duty)
{
    unsigned int time = 20000;

    IoTGpioSetOutputVal(IOT_IO_NAME_GPIO_2, IOT_GPIO_VALUE1);
    hi_udelay(duty);
    IoTGpioSetOutputVal(IOT_IO_NAME_GPIO_2, IOT_GPIO_VALUE0);
    hi_udelay(time - duty);
}

/* The steering gear is centered
1、依据角度与脉冲的关系，设置高电平时间为1500微秒
2、不断地发送信号，控制舵机居中
*/
void RegressMiddle(void)
{
    unsigned int angle = 1300;
    for (int i = 0; i < COUNT; i++) {
        SetAngle(angle);
    }
}

/* Turn 90 degrees to the right of the steering gear
1、依据角度与脉冲的关系，设置高电平时间为500微秒
2、不断地发送信号，控制舵机向右旋转90度
*/
/*  Steering gear turn right */
void EngineTurnRight(void)
{
    unsigned int angle = 500;
    for (int i = 0; i < COUNT; i++) {
        SetAngle(angle);
    }
}

/* Turn 90 degrees to the left of the steering gear
1、依据角度与脉冲的关系，设置高电平时间为2500微秒
2、不断地发送信号，控制舵机向左旋转90度
*/
/* Steering gear turn left */
void EngineTurnLeft(void)
{
    unsigned int angle = 2500;
    for (int i = 0; i < COUNT; i++) {
        SetAngle(angle);
    }
}

void S92RInit(void)
{
    // PWM舵机对应的GPIO是GPIO02
    // 初始化 GPIO02
    IoTGpioInit(IOT_IO_NAME_GPIO_2);
    // 设置GPIO02的管脚复用关系为GPIO
    IoSetFunc(IOT_IO_NAME_GPIO_2, IOT_IO_FUNC_GPIO_2_GPIO);
    // 设置GPIO02的方向为输出
    IoTGpioSetDir(IOT_IO_NAME_GPIO_2, IOT_GPIO_DIR_OUT);
}
