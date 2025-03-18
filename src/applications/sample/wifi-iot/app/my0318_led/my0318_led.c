// #include <stdio.h> //包含C语言标准头文件
// #include <ohos_init.h>//包含鸿蒙初始的头文件
// #include <unistd.h> // unix linux : ulseep(us)
// #include <hi_gpio.h>//鸿蒙的gpio的头文件
// #include <hi_io.h>//配置IO口的复用功能
// #include <cmsis_os2.h>

// void thread_func(void *arg){
//     //GPIO_10控制红灯
//     hi_gpio_init();//初始化
//     //配置GPIO_10作为普通的输入输出引脚
//     hi_io_set_func(HI_GPIO_IDX_10, HI_IO_FUNC_GPIO_10_GPIO);
//     //配置GPIO_10作输出功能
//     hi_gpio_set_dir(HI_GPIO_IDX_10, HI_GPIO_DIR_OUT);
//     //控制GPIO_10输出高低电平
//     int flag = 0;
//     while(1)
//     {
//         printf("in thread\n");
//         hi_gpio_set_ouput_val(HI_GPIO_IDX_10, flag);
//         usleep(1000000);
//         flag = !flag;
//     }
// }

// void myhello_func()
// {
//     osThreadAttr_t attr;
//     memset(&attr,0,sizeof(attr));
//     attr.name = "myhello";
//     attr.stack_size=4096;
//     attr.priority=11;

//     osThreadNew(thread_func,NULL,&attr);
// }

// SYS_RUN(myhello_func);


#include <stdio.h>
#include <string.h>
#include <ohos_init.h>
#include <unistd.h>
#include <hi_gpio.h>
#include <hi_io.h>
#include <cmsis_os2.h>

#define LED_RED_PIN   HI_GPIO_IDX_10  // 红灯控制
#define LED_GREEN_PIN HI_GPIO_IDX_11  // 绿灯控制
#define LED_BLUE_PIN  HI_GPIO_IDX_12  // 蓝灯控制

// 原有线程：控制单个红灯闪烁
void thread_func(void *arg){
    hi_gpio_init();
    hi_io_set_func(LED_RED_PIN, HI_IO_FUNC_GPIO_10_GPIO);
    hi_gpio_set_dir(LED_RED_PIN, HI_GPIO_DIR_OUT);
    
    int flag = 0;
    while(1)
    {
        printf("in thread\n");
        hi_gpio_set_ouput_val(LED_RED_PIN, flag);
        usleep(1000000);  // 1秒
        flag = !flag;
    }
}

// 新增线程：控制RGB颜色效果
void rgb_thread_func(void *arg) {
    hi_gpio_init();
    // 设置IO功能为GPIO
    hi_io_set_func(LED_RED_PIN, HI_IO_FUNC_GPIO_10_GPIO);
    hi_io_set_func(LED_GREEN_PIN, HI_IO_FUNC_GPIO_11_GPIO);
    hi_io_set_func(LED_BLUE_PIN, HI_IO_FUNC_GPIO_12_GPIO);

    // 设置为输出模式
    hi_gpio_set_dir(LED_RED_PIN, HI_GPIO_DIR_OUT);
    hi_gpio_set_dir(LED_GREEN_PIN, HI_GPIO_DIR_OUT);
    hi_gpio_set_dir(LED_BLUE_PIN, HI_GPIO_DIR_OUT);

    while (1) {
        // 红色
        hi_gpio_set_ouput_val(LED_RED_PIN, 1);
        hi_gpio_set_ouput_val(LED_GREEN_PIN, 0);
        hi_gpio_set_ouput_val(LED_BLUE_PIN, 0);
        usleep(5000);

        // 绿色
        hi_gpio_set_ouput_val(LED_RED_PIN, 0);
        hi_gpio_set_ouput_val(LED_GREEN_PIN, 1);
        hi_gpio_set_ouput_val(LED_BLUE_PIN, 0);
        usleep(5000);

        // 蓝色
        hi_gpio_set_ouput_val(LED_RED_PIN, 0);
        hi_gpio_set_ouput_val(LED_GREEN_PIN, 0);
        hi_gpio_set_ouput_val(LED_BLUE_PIN, 1);
        usleep(5000);

        // 黄色 (红+绿)
        hi_gpio_set_ouput_val(LED_RED_PIN, 1);
        hi_gpio_set_ouput_val(LED_GREEN_PIN, 1);
        hi_gpio_set_ouput_val(LED_BLUE_PIN, 0);
        usleep(5000);

        // 品红 (红+蓝)
        hi_gpio_set_ouput_val(LED_RED_PIN, 1);
        hi_gpio_set_ouput_val(LED_GREEN_PIN, 0);
        hi_gpio_set_ouput_val(LED_BLUE_PIN, 1);
        usleep(5000);

        // 青色 (绿+蓝)
        hi_gpio_set_ouput_val(LED_RED_PIN, 0);
        hi_gpio_set_ouput_val(LED_GREEN_PIN, 1);
        hi_gpio_set_ouput_val(LED_BLUE_PIN, 1);
        usleep(5000);

        // 白色 (红+绿+蓝)
        hi_gpio_set_ouput_val(LED_RED_PIN, 1);
        hi_gpio_set_ouput_val(LED_GREEN_PIN, 1);
        hi_gpio_set_ouput_val(LED_BLUE_PIN, 1);
        usleep(5000);

        // 熄灭所有颜色
        hi_gpio_set_ouput_val(LED_RED_PIN, 0);
        hi_gpio_set_ouput_val(LED_GREEN_PIN, 0);
        hi_gpio_set_ouput_val(LED_BLUE_PIN, 0);
        usleep(5000);
    }
}

// 系统启动函数：同时创建红灯和RGB LED控制线程
void myhello_func()
{
    osThreadAttr_t attr;
    memset(&attr, 0, sizeof(attr));
    attr.name = "myhello";
    attr.stack_size = 4096;
    attr.priority = 11;
    osThreadNew(thread_func, NULL, &attr);

    osThreadAttr_t attr_rgb;
    memset(&attr_rgb, 0, sizeof(attr_rgb));
    attr_rgb.name = "rgb_thread";
    attr_rgb.stack_size = 4096;
    attr_rgb.priority = 11;
    osThreadNew(rgb_thread_func, NULL, &attr_rgb);
}

SYS_RUN(myhello_func);
