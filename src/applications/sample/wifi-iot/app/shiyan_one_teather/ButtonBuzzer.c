#include <stdio.h>  //包含C语言标准头文件
#include <ohos_init.h> //包含鸿蒙初始的头文件
#include <cmsis_os2.h> //CMSIS接口标准头文件
#include <unistd.h> // unix linux : ulseep(us)
#include <hi_io.h>
#include <hi_gpio.h>

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

    //GPIO_09作普通的GPIO口（普通输入输出设备）
    hi_io_set_func(HI_GPIO_IDX_9, HI_IO_FUNC_GPIO_9_GPIO);
    //指定GPIO_09作输出
    hi_gpio_set_dir(HI_GPIO_IDX_9, HI_GPIO_DIR_OUT);

    int v;
    while (1)
    {
        //获取IO口电平
        hi_gpio_get_input_val(HI_GPIO_IDX_5, &v);
        if (0 == v) 
        {
            printf("key pressed\n");
            hi_gpio_set_ouput_val(HI_GPIO_IDX_9,0);
            //当按键按下时，循环等到按键松手为止,否则会触发很多下（后续补充）
            while (0 == v)
                {hi_gpio_get_input_val(HI_GPIO_IDX_5, &v);
                hi_gpio_set_ouput_val(HI_GPIO_IDX_9,1);}
        }
        else
        {
            hi_gpio_set_ouput_val(HI_GPIO_IDX_9,0);
        }
        usleep(200000);//休眠200ms
    }
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
