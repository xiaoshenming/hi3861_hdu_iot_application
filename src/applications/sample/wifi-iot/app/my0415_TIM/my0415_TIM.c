#include <stdio.h>
#include <ohos_init.h>
#include <unistd.h>
#include <hi_io.h>
#include <hi_gpio.h>
#include <iot_gpio.h>
#include <hi_time.h>
#include <cmsis_os2.h>

osTimerId_t timerID;
#define MY_IO HI_IO_NAME_GPIO_9

// 定时器回调函数
void timerFunc(void *arg)
{
    printf("time out\n");
    int v;
    IoTGpioGetOutputVal(MY_IO, &v);  // 获取当前GPIO状态
    IoTGpioSetOutputVal(MY_IO, !v);  // 翻转GPIO状态
}

// 主初始化函数
void myhello_func()
{
    printf("myhello test\n");
    IoTGpioInit(MY_IO);  // 初始化GPIO
    hi_io_set_func(MY_IO, HI_IO_FUNC_GPIO_9_GPIO);  // 设置GPIO功能
    IoTGpioSetDir(MY_IO, IOT_GPIO_DIR_OUT);  // 设置为输出模式
    IoTGpioSetOutputVal(MY_IO, 1);  // 初始输出高电平

    // 创建周期性定时器
    timerID = osTimerNew(timerFunc, osTimerPeriodic, NULL, NULL);
    osTimerStart(timerID, 100*3);  // 启动定时器，周期300ms
}

// 系统运行宏
SYS_RUN(myhello_func);