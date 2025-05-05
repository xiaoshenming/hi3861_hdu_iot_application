#include <stdio.h>
#include <ohos_init.h>
#include <cmsis_os2.h> //CMSIS接口标准头文件
#include <unistd.h>    // unix linux : ulseep(us)
#include <hi_gpio.h>   // GPIO驱动头文件
#include <hi_io.h>     // IO驱动头文件
//蜂鸣器不响

// void myhello_func()
// {
//     printf("myhello test\n");
//     //LS2蜂鸣器 -> GPIO_09
//     hi_gpio_init();
//     hi_io_set_func(HI_GPIO_IDX_9, HI_IO_FUNC_GPIO_9_GPIO);
//     hi_gpio_set_dir(HI_GPIO_IDX_9, HI_GPIO_DIR_OUT);

//     while (1)
//     {
//         hi_gpio_set_ouput_val(HI_GPIO_IDX_9,1);
//         usleep(5000000);//500ms
//         hi_gpio_set_ouput_val(HI_GPIO_IDX_9,0);
//         usleep(5000000);//500ms
//     }
// }

// 空的中断处理函数
void key_isr(void *arg)
{
    int v;
    hi_gpio_get_input_val(HI_GPIO_IDX_5, &v);
    if(v == 0)
    {
        printf("key pressed\n");
        hi_gpio_set_ouput_val(HI_GPIO_IDX_9,1);
    }
    else
    {
        printf("key released\n");
        hi_gpio_set_ouput_val(HI_GPIO_IDX_9, 0); // 蜂鸣器停
    }
}

void task_func(void *arg)
{
    // GPIO初始化
    hi_gpio_init();
    
    // 配置GPIO5为普通IO口
    hi_io_set_func(HI_GPIO_IDX_5, HI_IO_FUNC_GPIO_5_GPIO);
    
    // 配置为输入模式
    hi_gpio_set_dir(HI_GPIO_IDX_5, HI_GPIO_DIR_IN);
    
    // 配置上拉电阻
    hi_io_set_pull(HI_GPIO_IDX_5, HI_IO_PULL_UP);
    
    // 配置按钮消抖
    hi_io_set_schmitt(HI_GPIO_IDX_5, HI_TRUE);
    
    // 配置中断
    hi_gpio_register_isr_function(HI_GPIO_IDX_5, 
                                 HI_INT_TYPE_EDGE,
                                 HI_GPIO_EDGE_FALL_LEVEL_LOW | HI_GPIO_EDGE_RISE_LEVEL_HIGH, 
                                 key_isr, 
                                 NULL);
}

void myhello_func()
{
    // 设置任务属性
    osThreadAttr_t attr;
    memset(&attr, 0, sizeof(attr));
    attr.name = "mytask";
    attr.stack_size = 4096;
    attr.priority = osPriorityBelowNormal7;

    // 创建任务
    osThreadNew(task_func, NULL, &attr);
}

SYS_RUN(myhello_func);
