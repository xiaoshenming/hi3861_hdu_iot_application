#include <stdio.h>
#include <string.h>
#include <ohos_init.h>
#include <unistd.h>
#include "config.h"

// 定义全局温湿度变量
float g_temperature = 0;
float g_humidity = 0;
#include <hi_io.h>
#include <hi_gpio.h>
#include <iot_gpio.h>
#include <hi_time.h>
#include <cmsis_os2.h>
#include <hi_adc.h>
#include <iot_i2c.h>

#define AHT20_SDA  HI_IO_NAME_GPIO_13   //传感器数据引脚
#define AHT20_SCL  HI_IO_NAME_GPIO_14   //传感器时钟引脚
#define DEV_ADDR   0x38

void aht20_init()//初始化
{
    IoTGpioInit(AHT20_SCL);
    IoTGpioInit(AHT20_SDA);
    //配置IO口由I2C控制器使用
    hi_io_set_func(AHT20_SDA, HI_IO_FUNC_GPIO_13_I2C0_SDA);
    hi_io_set_func(AHT20_SCL, HI_IO_FUNC_GPIO_14_I2C0_SCL);
    //配置上拉
    hi_io_set_pull(AHT20_SDA, HI_IO_PULL_UP);
    hi_io_set_pull(AHT20_SCL, HI_IO_PULL_UP);   
    //初始化第0个I2C控制器
    IoTI2cInit(0,  100000);//I2C传输率100KHz
}

void aht20_check()
{
    char data = 0x71;
    if (0 != IoTI2cWrite(0, DEV_ADDR<<1|0, &data, 1))
    {
        printf("write 0x71 failed\n");
        return;
    }
    if (0 != IoTI2cRead(0, DEV_ADDR<<1|1, &data, 1 ))
    {
        printf("read1 failed\n");
        return;
    }
    if ((data & (1<<3)) == 0) // 检查校准位是否为1
    {
        // 发送初始化命令0xBE
        char cmd[] = {0xBE, 0x08, 0x00};
        if (0 != IoTI2cWrite(0, DEV_ADDR<<1|0, cmd, 3))
        {
            printf("write 0xbe failed\n");
            return;
        }
        usleep(10000); // 等待10ms
    }
}
void aht20_getdata()
{
    /* 直接发送 0xAC 命令（触发测量），此命令参数有两个字节，
       第一个字节为 0x33，第二个字节为 0x00。 */
    char cmd[3] = {0xAC, 0x33, 0x00};
    if(0 != IoTI2cWrite(0, DEV_ADDR<<1|0, cmd, 3))
    {
        printf("write 0xAC failed\n");
        return;
    }

    // 等待 80ms 待测量完成
    usleep(80000);

    // 读取 7 字节数据
    unsigned char data[7];
    if(0 != IoTI2cRead(0, DEV_ADDR<<1|1, data, 7))
    {
        printf("read data failed\n");
        return;
    }

    // 如果读取状态字 Bit[7] 为 0，表示测量完成（设备闲）
    if(data[0] & (1<<7))
    {
        printf("aht20 busy\n");
        return;
    }

    /* 温度和湿度 20 位，8+4+8
       湿度 data[1] data[2] data[3] 的高 4 位 */
    unsigned int humid = (data[1]<<12)|(data[2]<<4)|(data[3]>>4);
    /* 温度 data[3] 的低 4 位，data[4], data[5] */
    unsigned int temp = ((data[3]&0xf)<<16)|(data[4]<<8)|(data[5]);

    // 转换 1<<20 表示 20 次方的意思
    float fhumid = (float)humid/(1<<20)*100;
    float ftemp = (float)temp/(1<<20)*200-50;
    // 更新全局温湿度变量
    g_temperature = ftemp;
    g_humidity = fhumid;
    printf("temp: %f , humid: %f\n", ftemp, fhumid);
}

/* 添加任务函数 */
void task_func(void *arg)
{
    aht20_init();   // 初始化I2C
    aht20_check();  // 检查传感器状态
    
    while (1) 
    {
        aht20_getdata();    // 获取数据
        sleep(1);
    }
}
void myhello_func()
{
    printf("my IIC test\n");
    osDelay(100);
    osThreadAttr_t attr;
    memset(&attr,0,sizeof(attr));
    attr.name = "myhello";
    attr.stack_size = 4096;
    attr.priority = osPriorityAboveNormal1;
    osThreadNew(task_func,NULL,&attr);
}

SYS_RUN(myhello_func);
