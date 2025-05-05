#include <stdio.h> //包含C语言标准头文件
#include <ohos_init.h>//包含鸿蒙初始的头文件
#include <unistd.h> // unix linux : ulseep(us)
#include <hi_gpio.h>//鸿蒙的gpio的头文件
#include <hi_io.h>//配置IO口的复用功能

//万一厂商没有给我们电路图，需要自行确认灯在哪个IO口
//看输出到哪个IO口的时候灯有变化

void myhello_func()
{
    printf("hello\n");

    hi_gpio_init();//GPIO模块功能初始化

    for (int i = 0;i < 15; i++) 
    {//i表示第几个IO口: HI_GPIO_IDEX_i
        if ((3==i) || (4==i))
            continue; //因GPIO3 GPIO4作为COM口的输出引脚,所以不能改变原功能

        //配置第i个IO口的普通IO功能
        hi_io_set_func(i, 0);
        //配置第i个IO口输出功能
        hi_gpio_set_dir(i, HI_GPIO_DIR_OUT);
        //配置第i个IO口输出高低电平
        printf("gpio: %d\n", i);//输出信息提示第几个IO
        for (int j = 0; j < 10; j++) 
        {
            //用j表示奇偶，奇数是1，偶数是0
            hi_gpio_set_ouput_val(i, j%2);
            usleep(500000);//参数是us,休眠0.5秒
        }
    }
}

SYS_RUN(myhello_func);