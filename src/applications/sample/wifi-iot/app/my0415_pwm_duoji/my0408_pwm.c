#include <stdio.h>
#include <ohos_init.h>
#include <unistd.h>
#include <hi_io.h>
#include <hi_gpio.h>
#include <iot_gpio.h>
#include <hi_time.h>

/* 舵机控制引脚定义 
 * 红色--5V 灰色--GND 
 * 橙色信号线--GPIO_10 */
#define MY_IO HI_GPIO_IDX_10

void moveMotor(int duty) // duty表示高电平时间（微秒）
{
    for(int i=0; i<10; i++) { // 发送10个相同PWM信号确保舵机响应
        printf("motor %d test\n", i);
        hi_gpio_set_output_val(MY_IO, 1); // 高电平
        hi_udelay(duty);
        hi_gpio_set_output_val(MY_IO, 0); // 低电平
        hi_udelay(20000 - duty); // PWM周期20ms
    }
}

void myhello_func()
{
    printf("PWM servo test\n");

    // 初始化GPIO
    hi_gpio_init();
    hi_io_set_func(MY_IO, HI_IO_FUNC_GPIO_10_GPIO); // 设置引脚功能
    hi_gpio_set_dir(MY_IO, HI_GPIO_DIR_OUT); // 配置为输出

    while(1) {
        // 不同占空比测试（单位：微秒）
        moveMotor(500);   // 0°位置（0.5ms高电平）
        usleep(1000000);  // 延时1秒
        
        moveMotor(1000);  // 45°位置（1ms高电平）
        usleep(1000000);
        
        moveMotor(1500);   // 90°位置（1.5ms高电平）
        usleep(1000000);
        
        moveMotor(2500);   // 180°位置（2.5ms高电平）
        usleep(1000000);
    }
}