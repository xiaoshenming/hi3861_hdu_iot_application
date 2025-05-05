#include <stdio.h>
#include <ohos_init.h>
#include <cmsis_os2.h> //CMSIS接口标准头文件
#include <unistd.h>    // unix linux : ulseep(us)
#include <hi_gpio.h>   // GPIO驱动头文件
#include <hi_io.h>     // PWM驱动头文件

void thread_func(void *arg)
{
    hi_gpio_init();  // 初始化GPIO驱动
    hi_io_set_func(HI_GPIO_IDX_10,HI_IO_FUNC_GPIO_10_GPIO); // 设置GPIO10为GPIO功能
    hi_gpio_set_dir(HI_GPIO_IDX_10,HI_GPIO_DIR_OUT); // 设置GPIO10为输出模式
    int flag = 0;
    while (1)
    {
        printf("Hello World!\n");
        hi_gpio_set_ouput_val(HI_GPIO_IDX_10,flag); // 输出GPIO10的状态
        usleep(1000);
        flag =!flag;
    }
}
void myhello_func()
{
    osThreadAttr_t attr;
    memset(&attr, 0, sizeof(attr));
    attr.name = "myhello";
    attr.cb_size = 4096;
    attr.priority = 11;
    osThreadNew(thread_func, NULL, &attr);
}

SYS_RUN(myhello_func);
