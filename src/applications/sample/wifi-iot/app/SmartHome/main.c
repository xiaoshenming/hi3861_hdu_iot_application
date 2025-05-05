#include <stdio.h>
#include <string.h>
#include <ohos_init.h>
#include <unistd.h>
#include <hi_io.h>
#include <hi_gpio.h>
#include <iot_gpio.h>
#include <hi_time.h>
#include <cmsis_os2.h>
#include <hi_adc.h>

#define MY_IO HI_IO_NAME_GPIO_9  // GPIO引脚号
#define MYIO_FUNC HI_IO_FUNC_GPIO_9_GPIO  // GPIO功能号

void task_func()
{
    printf("in task func\n");
    IoTGpioInit(MY_IO);
    hi_io_set_func(MY_IO, MYIO_FUNC);
    IoTGpioSetDir(MY_IO, IOT_GPIO_DIR_IN); //输入

    hi_u16 v; // 存放读取的值
    while(1)
    {
    hi_adc_read(HI_ADC_CHANNEL_4, &v,HI_ADC_EQU_MODEL_8,
        HI_ADC_CUR_BAIS_DEFAULT,10); // 读取ADC数据
    // 最后延迟10个周期，等电压值比较稳定，再采集数据
    printf("sensor: %d %f v\n",v, hi_adc_convert_to_voltage(v));
    usleep(1000000);// 秒
    }
}

void myhello_func()
{
    printf("my hello test ADC\n");

    osThreadAttr_t attr;
    memset(&attr,0,sizeof(attr));
    attr.name = "myhello";
    attr.stack_size = 4096;
    attr.priority = osPriorityAboveNormal1;
    osThreadNew(task_func,NULL,&attr);
}

SYS_RUN(myhello_func);

