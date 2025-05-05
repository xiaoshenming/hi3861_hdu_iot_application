#include <stdio.h>  //包含C语言标准头文件
#include <ohos_init.h> //包含鸿蒙初始的头文件
#include <cmsis_os2.h> //CMSIS接口标准头文件
#include <unistd.h> // unix linux : ulseep(us)
#include <hi_io.h>
#include <hi_gpio.h>

//按键中断处理函数
void key_isr(void *arg)
{
    int v;
    //读取IO口的电平状态
    hi_gpio_get_input_val(HI_GPIO_IDX_5, &v);
    if (0 == v)  //低电平按键按下
        printf("key pressed\n");
    else  //高电平按键松开
        printf("key released\n");
}

//自定义任务执行函数
void task_func(void *arg)
{ 
    //循环检查按键是否按下
    hi_gpio_init(); //鸿蒙系统GPIO初始化

    //配置GPIO5作为普通IO口用途
    hi_io_set_func(HI_GPIO_IDX_5, HI_IO_FUNC_GPIO_5_GPIO);
    //配置输入
    hi_gpio_set_dir(HI_GPIO_IDX_5, HI_GPIO_DIR_IN);
    //配置上拉，让IO默认处于高电平状态
    hi_io_set_pull(HI_GPIO_IDX_5, HI_IO_PULL_UP);

    //配置按键抖动处理（后面补）
    hi_io_set_schmitt(HI_GPIO_IDX_5, HI_TRUE);
    //注册按键的中断处理,上升沿或下降沿时触发中断（有电平的变化和边沿的变化，我们用的是边沿的变化）
    //可以先输入hi_gpio_int_type看到库里面有的内容：HI_INT_TYPE_LEVEL和HI_INT_TYPE_EDGE
    //可以先输入hi_gpio_int_polarity看到库里面有的内容：选择用上升沿和下降沿触发,我们两个都写表示都触发中断
    hi_gpio_register_isr_function(HI_GPIO_IDX_5, HI_INT_TYPE_EDGE,
        HI_GPIO_EDGE_FALL_LEVEL_LOW|HI_GPIO_EDGE_RISE_LEVEL_HIGH, key_isr, NULL);
}


void myhello_func()
{
    //任务的设置
    osThreadAttr_t attr; //attr结构体变量配置任务属性
    memset(&attr, 0, sizeof(attr));  //attr结构体变量清零属性值
    attr.name = "mytask";//设置任务名
    attr.stack_size = 4096;//设置任务最大使用4KB的栈
    //attr.priority = 11;  //任务的优先级别: 8 ~ 55, 值越大级别越高
    attr.priority = osPriorityBelowNormal7;

    //创建任务
    osThreadNew(task_func, NULL, &attr);
}

SYS_RUN(myhello_func);
