/*
    HCSR05 超声波模块的相关API接口
*/
#include <stdio.h>
#include <stdlib.h>

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_gpio.h"
#include "hi_io.h"
#include "gyro.h"
#include "ssd1306.h"
#include "iot_errno.h"
#include "iot_gpio_ex.h"
#include "pca9555.h"
#include "hcsr04.h"

#define CAR_TURN_LEFT                     (0)
#define CAR_TURN_RIGHT                    (1)

CAR_DRIVE car_drive = { 0 };

int g_CarStarted = 0;

#define MASK_BUTTON1        (0x10)
#define MASK_BUTTON2        (0x08)
#define MASK_BUTTON3        (0x04)
#define YAW                 (90.0)
#define DISTANCE            (15.0)

ENUM_MODE g_mode = MODE_ON_OFF;

void init_ctrl_algo(void)
{
    memset(car_drive, 0, sizeof(CAR_DRIVE));
    car_drive.LeftForward = 10;
    car_drive.RightForward = 10;
    car_drive.TurnLeft = 30;
    car_drive.TurnRight = 30;
    car_drive.yaw = YAW;
    car_drive.distance = DISTANCE;
}

void init_oled_mode(void)
{
    g_mode = MODE_ON_OFF;
    ssd1306_ClearOLED();
    ssd1306_printf("LF:%d, RF:%d", car_drive.LeftForward, car_drive.RightForward);
    ssd1306_printf("TL:%d, RT:%d", car_drive.TurnLeft, car_drive.TurnRight);
    ssd1306_printf("yaw2:%.02f", car_drive.yaw);
    ssd1306_printf("distance:%.2f", car_drive.distance);
    ssd1306_printf("press btn2/3 to start");
}

void ButtonPressProc(uint8_t ext_io_val)
{
    static uint8_t ext_io_val_d = 0xFF;
    uint8_t diff;
    bool button1_pressed, button2_pressed, button3_pressed;
    
    diff = ext_io_val ^ ext_io_val_d;
    button1_pressed = ((diff & MASK_BUTTON1) && ((ext_io_val & MASK_BUTTON1) == 0)) ? true : false;
    button2_pressed = ((diff & MASK_BUTTON2) && ((ext_io_val & MASK_BUTTON2) == 0)) ? true : false;
    button3_pressed = ((diff & MASK_BUTTON3) && ((ext_io_val & MASK_BUTTON3) == 0)) ? true : false;
    ssd1306_ClearOLED();
    if (button1_pressed) {
        g_mode = (g_mode >= (MODE_END - 1)) ? 0 : (g_mode + 1);
        switch (g_mode) {
            case MODE_ON_OFF:
                ssd1306_printf("LF:%d, RF:%d", car_drive.LeftForward, car_drive.RightForward);
                ssd1306_printf("TL:%d, TR:%d", car_drive.TurnLeft, car_drive.TurnRight);
                ssd1306_printf("yaw:%.2f", car_drive.yaw);
                ssd1306_printf("distance:%.2f", car_drive.distance);
                ssd1306_printf("press btn2/3 to start");
                break;
            case MODE_SET_LEFT_FORWARD:
                ssd1306_printf("Set LForward=%d", car_drive.LeftForward);
                break;
            case MODE_SET_RIGHT_FORWARD:
                ssd1306_printf("Set RForward=%d", car_drive.RightForward);
                break;
            case MODE_SET_TURN_LEFT:
                ssd1306_printf("Set TurnLeft=%d", car_drive.TurnLeft);
                break;
            case MODE_SET_TURN_RIGHT:
                ssd1306_printf("Set TurnRight=%d", car_drive.TurnRight);
                break;
            case MODE_SET_YAW:
                ssd1306_printf("Set yaw = %.2f", car_drive.yaw);
                break;
            case MODE_SET_DISTANCE:
                ssd1306_printf("Set distance=%.2f", car_drive.distance);
                break;
            default:
                init_oled_mode();
                break;
        }
    } else if (button2_pressed || button3_pressed) {
        switch (g_mode) {
            case MODE_ON_OFF:
                g_CarStarted = !g_CarStarted;
                ssd1306_ClearOLED();
                ssd1306_printf(g_CarStarted ? "start" : "stop");
                break;
            case MODE_SET_LEFT_FORWARD:
                car_drive.LeftForward += ((button2_pressed) ? -1 : 1);
                ssd1306_printf("LeftForward=%d", car_drive.LeftForward); 
                break;
            case MODE_SET_RIGHT_FORWARD:
                car_drive.RightForward += (button2_pressed ? -1 : 1);
                ssd1306_printf("RightForward=%d", car_drive.RightForward); 
                break;
            case MODE_SET_TURN_LEFT:
                car_drive.TurnLeft += ((button2_pressed) ? -1 : 1);
                ssd1306_printf("TurnLeft=%d", car_drive.TurnLeft); 
                break;
            case MODE_SET_TURN_RIGHT:
                car_drive.TurnRight += ((button2_pressed) ? -1 : 1);
                ssd1306_printf("TurnRight=%d", car_drive.TurnRight); 
                break;
            case MODE_SET_YAW:
                car_drive.yaw += ((button2_pressed) ? -0.1 : 0.1);
                ssd1306_printf("yaw =%.2f", car_drive.yaw);
                break;
            case MODE_SET_DISTANCE:
                car_drive.distance += (button2_pressed ? -0.1 : 0.1);
                ssd1306_printf("distance=%.2f", car_drive.distance);
                break;
            default:
                break;
        }
    }
    
    ext_io_val_d = ext_io_val;
}

void Hcsr04Init(void)
{
    // 设置超声波Echo为输入模式
    // 设置GPIO8功能（设置为GPIO功能）
    IoSetFunc(IOT_IO_NAME_GPIO_8, IOT_IO_FUNC_GPIO_8_GPIO);
    // 设置GPIO8为输入方向
    IoTGpioSetDir(IOT_IO_NAME_GPIO_8, IOT_GPIO_DIR_IN);

    // 设置GPIO7功能（设置为GPIO功能）
    IoSetFunc(IOT_IO_NAME_GPIO_7, IOT_IO_FUNC_GPIO_7_GPIO);
    // 设置GPIO7为输出方向
    IoTGpioSetDir(IOT_IO_NAME_GPIO_7, IOT_GPIO_DIR_OUT);
}

float GetDistance(void)
{
    // 定义变量
    static unsigned long start_time = 0, time = 0;
    float distance = 0.0;
    static char line[32] = {0};
    IotGpioValue value = IOT_GPIO_VALUE0;
    unsigned int flag = 0;

    // 设置GPIO7输出低电平
    /* 给trig发送至少10us的高电平脉冲，以触发传感器测距 */
    IoTGpioSetOutputVal(IOT_IO_NAME_GPIO_7, IOT_GPIO_VALUE1);
    // 延时函数（设置高电平持续时间）
    hi_udelay(20);
    // 设置GPIO7输出高电平
    IoTGpioSetOutputVal(IOT_IO_NAME_GPIO_7, IOT_GPIO_VALUE0);
    /* 计算与障碍物之间的距离 */
    while (1) {
        // 获取GPIO8的输入电平状态
        IoTGpioGetInputVal(IOT_IO_NAME_GPIO_8, &value);
        //判断GPIO8的输入电平是否为高电平并且flag为0
        if ( value == IOT_GPIO_VALUE1 && flag == 0) {
            // 获取系统时间
            start_time = hi_get_us();
            // 将flag设置为1
            flag = 1;
        }
        //判断GPIO8的输入电平是否为低电平并且flag为1
        if (value == IOT_GPIO_VALUE0 && flag == 1) {
            // 获取高电平持续时间
            time = hi_get_us() - start_time;
            break;
        }
    }
    // 计算距离障碍物距离（340米/秒 转换为 0.034厘米/微秒）
    distance = time * 0.034 / 2;
    return distance;
}

/*Judge steering gear*/
unsigned int engine_go_where(hi_void)
{
    float left_distance = 0.0;
    float right_distance = 0.0;
    /*舵机往左转动测量左边障碍物的距离*/

    EngineTurnLeft();
    TaskMsleep(200);
    left_distance = GetDistance();
    TaskMsleep(200);
    /*归中*/
    RegressMiddle();
    TaskMsleep(200);

    /*舵机往右转动测量右边障碍物的距离*/
    EngineTurnRight();
    TaskMsleep(200);
    right_distance = GetDistance();
    TaskMsleep(200);
    /*归中*/
    RegressMiddle();

    if (left_distance > right_distance) {
        return CAR_TURN_LEFT;
    } else {
        return CAR_TURN_RIGHT;
    }
}

/*根据障碍物的距离来判断小车的行走方向
1、距离大于等于15cm继续前进
2、距离小于15cm，先停止再后退0.1s,继续进行测距,再进行判断
*/
/*Judge the direction of the car*/
unsigned int car_where_to_go(float distance)
{
    // if (distance < car_drive.distance) {
    //     car_backward(car_drive.LeftForward, car_drive.RightForward);
    //     TaskMsleep(100);
    //     car_stop();
    //     unsigned int ret = engine_go_where();
    //     if (ret == CAR_TURN_LEFT) {
            while (1) {
                Lsm_Get_RawAcc();
                float ret = GetYaw();
                ssd1306_printf("yaw =%.2f", ret);
                // printf("gyropos.yaw = %0.2f\r\n", gyropos->yaw);
            }
        // }
        //     // car_left(car_drive.TurnRight);
            // TaskMsleep(500);
//         } else if (ret == CAR_TURN_RIGHT) {
//             car_right( car_drive.TurnLeft);
//             TaskMsleep(500);
//         }
//         car_stop();
//     } else {
//         car_forward(car_drive.LeftForward, car_drive.RightForward);
    // }
}

/* 超声波避障 */
void ultrasonic_demo()
{
    float m_distance = 0.0;
    // regress_middle();
    /*获取前方物体的距离*/
    m_distance = GetDistance();
    car_where_to_go(m_distance);
    TaskMsleep(20);
}

void UltrasonicDemoTask()
{
    InitPCA9555();
    S92RInit();
    GA12N20Init();
    Hcsr04Init();
    LSM6DS_Init();
    TaskMsleep(100);
    init_ctrl_algo();
    init_oled_mode();
    PCA_RegisterEventProcFunc(ButtonPressProc);
    while (1) {
        if (g_CarStarted) {
            ultrasonic_demo();
        } else {
            car_stop();
        }
    }
}

void UltrasonicSampleEntry(void)
{
    // 初始化超声波传感器
    osThreadAttr_t attr;
    IoTWatchDogDisable();
    attr.name = "UltrasonicDemoTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 1024 * 5; /* 堆栈大小为1024 */
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)UltrasonicDemoTask, NULL, &attr) == NULL) {
        printf("[UltrasonicDemoTask] Failed to create UltrasonicDemoTask!\n");
    }
}
APP_FEATURE_INIT(UltrasonicSampleEntry);