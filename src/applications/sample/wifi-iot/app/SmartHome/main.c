/*
 * Copyright (c) 2024 智能家居系统
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * 智能家居系统 - 主控制程序
 * 功能：集成多种传感器和执行器，实现智能家居控制
 * 包含模块：
 * 1. LED灯光控制模块 - 实现自动照明和指示灯功能
 * 2. 按键输入模块 - 用户交互界面
 * 3. 蜂鸣器警报模块 - 安全警报系统
 * 4. 温度传感器模块 - 环境监测
 * 5. 舵机控制模块 - 智能窗帘/门控制
 * 6. 主控制模块 - 协调各个功能模块工作
 */

#include <stdio.h>
#include <string.h>
#include <ohos_init.h>
#include <cmsis_os2.h>
#include <unistd.h>
#include <hi_io.h>
#include <hi_gpio.h>
#include <iot_gpio.h>
#include <hi_adc.h>
#include <hi_time.h>
#include <iot_pwm.h>
#include <hi_pwm.h>

/* 引脚定义 */
#define LED_PIN         HI_GPIO_IDX_9    // LED灯引脚
#define BUTTON_PIN      HI_GPIO_IDX_5    // 按键引脚
#define BUZZER_PIN      HI_GPIO_IDX_9    // 蜂鸣器引脚（与LED共用）
#define SERVO_PIN       HI_GPIO_IDX_10   // 舵机控制引脚
#define TEMP_ADC_CHANNEL HI_ADC_CHANNEL_4 // 温度传感器ADC通道

/* 系统状态定义 */
#define MODE_NORMAL     0    // 正常模式
#define MODE_ALARM      1    // 警报模式
#define MODE_ENERGY     2    // 节能模式

/* PWM相关定义 */
#define PWM_CLOCK_SOURCE 160000000  // PWM时钟源频率
#define PWM_FREQ_NORMAL  2000      // 普通蜂鸣器频率
#define PWM_FREQ_ALARM   4000      // 警报蜂鸣器频率

/* 温度阈值定义 */
#define TEMP_HIGH       30.0       // 高温阈值（摄氏度）
#define TEMP_LOW        18.0       // 低温阈值（摄氏度）

/* 全局变量 */
static int g_systemMode = MODE_NORMAL;  // 系统当前模式
static int g_ledState = 0;              // LED当前状态
static float g_currentTemp = 25.0;      // 当前温度值
static osTimerId_t g_ledTimerId;        // LED控制定时器
static osTimerId_t g_tempTimerId;       // 温度检测定时器
static osTimerId_t g_servoTimerId;      // 舵机控制定时器
static osMutexId_t g_modeMutex;         // 模式切换互斥锁

/* 函数声明 */
static void InitAllHardware(void);
static void TemperatureSensorTask(void *arg);
static void LedControlTimerCallback(void *arg);
static void TempMonitorTimerCallback(void *arg);
static void ServoControlTimerCallback(void *arg);
static void ButtonIsrCallback(void *arg);
static void PlayAlarmTone(void);
static void StopAlarmTone(void);
static void ControlServo(int angle);
static float ReadTemperature(void);
static void SetSystemMode(int mode);
static void HandleHighTemperature(void);
static void HandleLowTemperature(void);
static void HandleNormalTemperature(void);

/**
 * @brief 按键中断处理函数
 * @param arg 参数指针
 */
static void ButtonIsrCallback(void *arg)
{
    (void)arg;
    int value;
    hi_gpio_get_input_val(BUTTON_PIN, &value);
    
    if (value == 0) { // 按键按下（低电平）
        printf("按键按下，切换系统模式\n");
        
        // 获取互斥锁
        osMutexAcquire(g_modeMutex, osWaitForever);
        
        // 循环切换系统模式
        g_systemMode = (g_systemMode + 1) % 3;
        printf("系统模式已切换为: %d\n", g_systemMode);
        
        // 根据新模式执行相应操作
        switch (g_systemMode) {
            case MODE_NORMAL:
                printf("进入正常模式\n");
                StopAlarmTone();
                break;
            case MODE_ALARM:
                printf("进入警报模式\n");
                PlayAlarmTone();
                break;
            case MODE_ENERGY:
                printf("进入节能模式\n");
                StopAlarmTone();
                // 关闭LED
                hi_gpio_set_ouput_val(LED_PIN, 0);
                break;
            default:
                break;
        }
        
        // 释放互斥锁
        osMutexRelease(g_modeMutex);
    }
}

/**
 * @brief 温度传感器任务函数
 * @param arg 参数指针
 */
static void TemperatureSensorTask(void *arg)
{
    (void)arg;
    
    while (1) {
        // 读取当前温度
        g_currentTemp = ReadTemperature();
        printf("当前温度: %.2f°C\n", g_currentTemp);
        
        // 获取互斥锁
        osMutexAcquire(g_modeMutex, osWaitForever);
        
        // 根据温度执行相应操作
        if (g_systemMode == MODE_NORMAL) {
            if (g_currentTemp > TEMP_HIGH) {
                HandleHighTemperature();
            } else if (g_currentTemp < TEMP_LOW) {
                HandleLowTemperature();
            } else {
                HandleNormalTemperature();
            }
        }
        
        // 释放互斥锁
        osMutexRelease(g_modeMutex);
        
        // 每2秒检测一次温度
        osDelay(200);
    }
}

/**
 * @brief LED控制定时器回调函数
 * @param arg 参数指针
 */
static void LedControlTimerCallback(void *arg)
{
    (void)arg;
    
    // 获取互斥锁
    osMutexAcquire(g_modeMutex, osWaitForever);
    
    // 在正常模式和警报模式下闪烁LED
    if (g_systemMode != MODE_ENERGY) {
        // 切换LED状态
        g_ledState = !g_ledState;
        hi_gpio_set_ouput_val(LED_PIN, g_ledState);
        
        // 警报模式下闪烁更快
        if (g_systemMode == MODE_ALARM) {
            osTimerStart(g_ledTimerId, 300); // 300ms
        } else {
            osTimerStart(g_ledTimerId, 1000); // 1000ms
        }
    }
    
    // 释放互斥锁
    osMutexRelease(g_modeMutex);
}

/**
 * @brief 温度监控定时器回调函数
 * @param arg 参数指针
 */
static void TempMonitorTimerCallback(void *arg)
{
    (void)arg;
    
    // 读取当前温度
    g_currentTemp = ReadTemperature();
    printf("定时温度检测: %.2f°C\n", g_currentTemp);
    
    // 获取互斥锁
    osMutexAcquire(g_modeMutex, osWaitForever);
    
    // 在正常模式下根据温度调整系统行为
    if (g_systemMode == MODE_NORMAL) {
        if (g_currentTemp > TEMP_HIGH) {
            printf("温度过高警告!\n");
            // 可以在这里添加高温处理逻辑
        } else if (g_currentTemp < TEMP_LOW) {
            printf("温度过低警告!\n");
            // 可以在这里添加低温处理逻辑
        }
    }
    
    // 释放互斥锁
    osMutexRelease(g_modeMutex);
}

/**
 * @brief 舵机控制定时器回调函数
 * @param arg 参数指针
 */
static void ServoControlTimerCallback(void *arg)
{
    (void)arg;
    static int servoPosition = 0; // 0-180度
    static int direction = 1;     // 1:增加角度, -1:减少角度
    
    // 获取互斥锁
    osMutexAcquire(g_modeMutex, osWaitForever);
    
    // 在节能模式下，关闭窗帘（舵机到0度）
    if (g_systemMode == MODE_ENERGY) {
        ControlServo(0);
    } 
    // 在警报模式下，打开窗帘（舵机到180度）
    else if (g_systemMode == MODE_ALARM) {
        ControlServo(180);
    }
    // 在正常模式下，根据温度调整窗帘位置
    else {
        // 温度高于阈值，打开窗帘散热
        if (g_currentTemp > TEMP_HIGH) {
            servoPosition += 10 * direction;
            if (servoPosition >= 180) {
                servoPosition = 180;
                direction = -1;
            } else if (servoPosition <= 0) {
                servoPosition = 0;
                direction = 1;
            }
            ControlServo(servoPosition);
        }
        // 温度正常，窗帘保持在90度
        else {
            ControlServo(90);
        }
    }
    
    // 释放互斥锁
    osMutexRelease(g_modeMutex);
}

/**
 * @brief 播放警报音
 */
static void PlayAlarmTone(void)
{
    // 配置蜂鸣器为PWM输出
    hi_io_set_func(BUZZER_PIN, HI_IO_FUNC_GPIO_9_PWM0_OUT);
    IoTGpioInit(BUZZER_PIN);
    IoTGpioSetDir(BUZZER_PIN, IOT_GPIO_DIR_OUT);
    
    // 初始化PWM
    IoTPwmInit(0);
    hi_pwm_set_clock(PWM_CLK_XTAL);
    
    // 启动PWM，产生警报音
    int freq = PWM_CLOCK_SOURCE / PWM_FREQ_ALARM;
    IoTPwmStart(0, 50, freq); // 50%占空比
}

/**
 * @brief 停止警报音
 */
static void StopAlarmTone(void)
{
    // 停止PWM
    IoTPwmStop(0);
    
    // 重新配置为普通GPIO
    hi_io_set_func(BUZZER_PIN, HI_IO_FUNC_GPIO_9_GPIO);
    hi_gpio_set_dir(BUZZER_PIN, HI_GPIO_DIR_OUT);
}

/**
 * @brief 控制舵机转动到指定角度
 * @param angle 目标角度（0-180度）
 */
static void ControlServo(int angle)
{
    // 角度范围限制
    if (angle < 0) angle = 0;
    if (angle > 180) angle = 180;
    
    // 将角度转换为高电平时间（微秒）
    // 舵机通常使用0.5ms-2.5ms的脉冲宽度对应0-180度
    int pulseWidth = 500 + (angle * 2000 / 180);
    
    // 发送10个PWM信号确保舵机响应
    for (int i = 0; i < 10; i++) {
        hi_gpio_set_ouput_val(SERVO_PIN, 1); // 高电平
        hi_udelay(pulseWidth);
        hi_gpio_set_ouput_val(SERVO_PIN, 0); // 低电平
        hi_udelay(20000 - pulseWidth); // PWM周期20ms
    }
    
    printf("舵机已调整到 %d 度\n", angle);
}

/**
 * @brief 读取温度传感器数据
 * @return 当前温度值（摄氏度）
 */
static float ReadTemperature(void)
{
    hi_u16 adcValue;
    
    // 读取ADC值
    hi_adc_read(TEMP_ADC_CHANNEL, &adcValue, HI_ADC_EQU_MODEL_8, HI_ADC_CUR_BAIS_DEFAULT, 10);
    
    // 将ADC值转换为电压
    float voltage = hi_adc_convert_to_voltage(adcValue);
    
    // 将电压转换为温度（假设使用LM35温度传感器，其输出为10mV/°C）
    // 注意：实际应用中需要根据具体传感器调整转换公式
    float temperature = voltage * 100.0;
    
    return temperature;
}

/**
 * @brief 设置系统模式
 * @param mode 目标模式
 */
static void SetSystemMode(int mode)
{
    // 获取互斥锁
    osMutexAcquire(g_modeMutex, osWaitForever);
    
    // 设置新模式
    g_systemMode = mode;
    printf("系统模式已设置为: %d\n", mode);
    
    // 释放互斥锁
    osMutexRelease(g_modeMutex);
}

/**
 * @brief 处理高温情况
 */
static void HandleHighTemperature(void)
{
    printf("温度过高，启动散热措施\n");
    
    // 打开窗帘（舵机到180度）
    ControlServo(180);
    
    // 如果温度持续过高，可以触发警报
    static int highTempCount = 0;
    highTempCount++;
    
    if (highTempCount > 5) { // 连续5次高温读数
        printf("持续高温警报!\n");
        SetSystemMode(MODE_ALARM);
        highTempCount = 0;
    }
}

/**
 * @brief 处理低温情况
 */
static void HandleLowTemperature(void)
{
    printf("温度过低，启动保温措施\n");
    
    // 关闭窗帘（舵机到0度）
    ControlServo(0);
}

/**
 * @brief 处理正常温度情况
 */
static void HandleNormalTemperature(void)
{
    // 温度正常，窗帘保持在90度
    ControlServo(90);
}

/**
 * @brief 初始化所有硬件
 */
static void InitAllHardware(void)
{
    // 初始化GPIO
    hi_gpio_init();
    
    // 配置LED引脚
    hi_io_set_func(LED_PIN, HI_IO_FUNC_GPIO_9_GPIO);
    hi_gpio_set_dir(LED_PIN, HI_GPIO_DIR_OUT);
    hi_gpio_set_ouput_val(LED_PIN, 0); // 初始状态为关闭
    
    // 配置按键引脚
    hi_io_set_func(BUTTON_PIN, HI_IO_FUNC_GPIO_5_GPIO);
    hi_gpio_set_dir(BUTTON_PIN, HI_GPIO_DIR_IN);
    hi_io_set_pull(BUTTON_PIN, HI_IO_PULL_UP); // 上拉电阻
    hi_io_set_schmitt(BUTTON_PIN, HI_TRUE);   // 施密特触发器（消抖）
    
    // 注册按键中断
    hi_gpio_register_isr_function(
        BUTTON_PIN,
        HI_INT_TYPE_EDGE,
        HI_GPIO_EDGE_FALL_LEVEL_LOW | HI_GPIO_EDGE_RISE_LEVEL_HIGH,
        ButtonIsrCallback,
        NULL
    );
    
    // 配置舵机引脚
    hi_io_set_func(SERVO_PIN, HI_IO_FUNC_GPIO_10_GPIO);
    hi_gpio_set_dir(SERVO_PIN, HI_GPIO_DIR_OUT);
    
    // 初始化ADC
    // 注意：在实际应用中可能需要更多的ADC配置
    
    printf("硬件初始化完成\n");
}

/**
 * @brief 智能家居系统主函数
 */
void SmartHomeMain(void)
{
    printf("智能家居系统启动...\n");
    
    // 创建互斥锁
    g_modeMutex = osMutexNew(NULL);
    if (g_modeMutex == NULL) {
        printf("创建互斥锁失败!\n");
        return;
    }
    
    // 初始化硬件
    InitAllHardware();
    
    // 创建定时器
    g_ledTimerId = osTimerNew(LedControlTimerCallback, osTimerPeriodic, NULL, NULL);
    g_tempTimerId = osTimerNew(TempMonitorTimerCallback, osTimerPeriodic, NULL, NULL);
    g_servoTimerId = osTimerNew(ServoControlTimerCallback, osTimerPeriodic, NULL, NULL);
    
    // 启动定时器
    osTimerStart(g_ledTimerId, 1000);    // LED控制定时器，1秒
    osTimerStart(g_tempTimerId, 5000);   // 温度监控定时器，5秒
    osTimerStart(g_servoTimerId, 10000); // 舵机控制定时器，10秒
    
    // 创建温度传感器任务
    osThreadAttr_t attr;
    memset(&attr, 0, sizeof(attr));
    attr.name = "TempSensorTask";
    attr.stack_size = 4096;
    attr.priority = osPriorityNormal;
    
    osThreadId_t tempTaskId = osThreadNew(TemperatureSensorTask, NULL, &attr);
    if (tempTaskId == NULL) {
        printf("创建温度传感器任务失败!\n");
    }
    
    // 系统初始化完成，播放提示音
    PlayAlarmTone();
    osDelay(50); // 短暂延时
    StopAlarmTone();
    
    printf("智能家居系统初始化完成，进入正常工作状态\n");
}

// 注册系统启动函数
SYS_RUN(SmartHomeMain);
