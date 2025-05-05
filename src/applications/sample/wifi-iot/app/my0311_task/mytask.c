/*
 * mytask.c - GPIO控制示例程序
 * 功能：通过GPIO9引脚控制LED闪烁
 * 作者：Jjing
 * 日期：2024-04-17
 */

#include <stdio.h>
#include <ohos_init.h>
#include <cmsis_os2.h> //CMSIS接口标准头文件
#include <unistd.h>    // unix linux : ulseep(us)
#include <hi_gpio.h>   // GPIO驱动头文件
#include <hi_io.h>     // PWM驱动头文件
/**
 * @brief GPIO控制主函数
 * 初始化GPIO9为输出模式，循环切换高低电平实现LED闪烁
 */
void myhello_func()
{

    hi_gpio_init();  // 初始化GPIO驱动
    hi_io_set_func(HI_GPIO_IDX_9, HI_IO_FUNC_GPIO_9_GPIO);  // 设置GPIO9引脚功能
    hi_gpio_set_dir(HI_GPIO_IDX_9, HI_GPIO_DIR_OUT);  // 设置GPIO9为输出模式
    int flag = 0;  // 电平状态标志，0-低电平，1-高电平
    while (1) {
        hi_gpio_set_ouput_val(HI_GPIO_IDX_9, flag);  // 设置GPIO9输出电平
        usleep(1000000);  // 延时1秒
        flag=!flag;  // 切换电平状态
    }
}

SYS_RUN(myhello_func);
