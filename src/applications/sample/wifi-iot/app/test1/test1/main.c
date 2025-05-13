/*
 * Copyright (C) 2021 HiHope Open Source Organization .
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h> // 用于 strlen, 虽然对于 ssd1306_DrawString 不是严格必需的

#include "ohos_init.h"      // 用于 APP_FEATURE_INIT
#include "cmsis_os2.h"      // 用于 osThreadAttr_t, osThreadNew, osPriorityNormal, osDelay
#include "iot_gpio.h"       // 用于 IoTGpioInit
#include "iot_i2c.h"        // 用于 IoTI2cInit
#include "iot_errno.h"      // 用于 IOT_SUCCESS
#include "hi_io.h"          // 用于 hi_io_set_func 和 GPIO 功能定义

#include "ssd1306.h"        //核心 SSD1306 驱动函数

// --- 硬件配置 ---
// 重要提示：请为您的特定开发板验证并更新这些 GPIO 和 I2C 设置!
#define OLED_I2C_PORT     (0) // I2C 端口 (例如 0 或 1)
#define OLED_I2C_BAUDRATE (400 * 1000) // 400kHz, SSD1306 常用的波特率

// Hi3861 开发板上 I2C0 的示例 GPIO 引脚。
// 请替换为您的硬件对应的正确引脚。
#define OLED_GPIO_SDA     HI_IO_NAME_GPIO_13
#define OLED_GPIO_SCL     HI_IO_NAME_GPIO_14

// I2C0 的示例 GPIO 引脚功能设置。
// 请替换为您的硬件和所选引脚对应的正确功能。
#define OLED_I2C_SDA_FUNC HI_IO_FUNC_GPIO_13_I2C0_SDA
#define OLED_I2C_SCL_FUNC HI_IO_FUNC_GPIO_14_I2C0_SCL
// --- 硬件配置结束 ---

#define APP_THREAD_STACK_SIZE (10240) // 应用任务的堆栈大小
#define APP_THREAD_PRIORITY   osPriorityNormal // 应用任务的优先级

/**
 * @brief 初始化 GPIO, I2C 和 SSD1306 OLED 显示屏。
 *
 * @return 成功返回 0，失败返回 -1 (针对外设初始化，而非 SSD1306_Init 本身)。
 */
static int OledPeripheralInit(void) {
    uint32_t ret;

    // 初始化 I2C 的 GPIO 引脚
    if (IoTGpioInit(OLED_GPIO_SDA) != IOT_SUCCESS) {
        printf("初始化 SDA GPIO失败 (引脚 %d)!\n", OLED_GPIO_SDA);
        return -1;
    }
    if (IoTGpioInit(OLED_GPIO_SCL) != IOT_SUCCESS) {
        printf("初始化 SCL GPIO失败 (引脚 %d)!\n", OLED_GPIO_SCL);
        return -1;
    }

    // 设置 GPIO 引脚功能为 I2C
    ret = hi_io_set_func(OLED_GPIO_SDA, OLED_I2C_SDA_FUNC);
    if (ret != IOT_SUCCESS) {
        printf("设置 SDA 引脚功能失败! 错误码: %u\n", ret);
        return -1;
    }
    ret = hi_io_set_func(OLED_GPIO_SCL, OLED_I2C_SCL_FUNC);
    if (ret != IOT_SUCCESS) {
        printf("设置 SCL 引脚功能失败! 错误码: %u\n", ret);
        return -1;
    }
    
    // 初始化 I2C 控制器
    ret = IoTI2cInit(OLED_I2C_PORT, OLED_I2C_BAUDRATE);
    if (ret != IOT_SUCCESS) {
        printf("I2C 主机初始化失败! 错误码: %u\n", ret);
        return -1;
    }
    printf("I2C 主机在端口 %d 初始化成功。\n", OLED_I2C_PORT);

    // 在 SSD1306 初始化前短暂停顿，参考一些示例的做法
    usleep(20 * 1000); // 20毫秒延迟

    // 初始化 SSD1306 OLED 驱动
    // 根据之前的编译错误，ssd1306_Init() 是一个 void 函数。
    // 我们无法检查其返回值。我们假设它内部处理错误，
    // 或者成功的 I2C 初始化足以让它继续执行。
    ssd1306_Init(); 
    printf("SSD1306 OLED 初始化序列已启动。\n"); // 更改了消息以反映无返回检查

    // 初始化后清屏是一个好习惯
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen(); // 确保清屏命令已发送

    return 0; // 如果 I2C 和 GPIO 初始化通过，则返回成功。
}

/**
 * @brief OLED 显示应用的主任务。
 *
 * 此任务初始化 OLED，清屏，然后写入指定的字符串。
 * @param arg 任务参数 (未使用)。
 */
static void OledDisplayTask(void *arg) {
    (void)arg; // 标记参数为未使用

    printf("OledDisplayTask 已启动。\n");

    // 初始化 OLED 和外设
    if (OledPeripheralInit() != 0) {
        printf("OLED 外设初始化失败。正在退出任务。\n");
        return;
    }

    // 屏幕应已在 OledPeripheralInit 中 ssd1306_Init() 之后被清除
    // ssd1306_Fill(Black); // 现在这可能是多余的，但是安全的
    // ssd1306_UpdateScreen(); 

    // --- 显示 "hello world" ---
    // 设置光标到左上角 (x=0, y=0)
    ssd1306_SetCursor(0, 0); 
    ssd1306_DrawString("hello world", Font_7x10, White);
    printf("尝试绘制 'hello world'。\n");

    // --- 在新行上显示 "你好鸿蒙" ---
    // Font_7x10 字体高度为10像素。相应地定位下一行。
    // Y=12 提供了一个小的2像素间隙。根据需要调整。128x64 OLED 的最大 Y 值为 63。
    ssd1306_SetCursor(0, 12); 
    // 警告: Font_7x10 字体极不可能支持中文字符。
    // 对于 "你好鸿蒙"，此行很可能会显示乱码或不正确的输出。
    // // 需要带有中文字形的字体和适当的库支持才能正确显示。
    ssd1306_DrawString("你好鸿蒙", Font_7x10, White);
    printf("尝试绘制 '你好鸿蒙'。\n");
    
    // 在现有 "你好鸿蒙" 后添加新内容（假设 Y 轴仍有空间）
    ssd1306_SetCursor(0, 24); // 第三行 (24 = 12 + 12)
    ssd1306_DrawString("Status: OK", Font_7x10, White);

    ssd1306_SetCursor(0, 36); // 第四行 (36 = 24 + 12)
    ssd1306_DrawString("Temp: 25.5C", Font_7x10, White);

    ssd1306_SetCursor(0, 48); // 第五行 (48 = 36 + 12)
    ssd1306_DrawString("IP: 192.168.1.1", Font_7x10, White);

    // 确保更新屏幕（如果未调用过）
    // ssd1306_UpdateScreen();


    // 将缓冲区内容传输到 OLED 屏幕
    ssd1306_UpdateScreen();
    printf("OLED 屏幕已更新。请检查显示。\n");

    // 如果任务需要执行更多工作或保持运行，可以在此处循环。
    // 对于一次性显示，它也可以退出。
    // 在此示例中，让它打印一条消息，然后带延迟循环。
    int count = 0;
    while (1) {
        printf("OLED 显示任务存活... 循环 %d\n", ++count);
        osDelay(5000); // 延迟5秒 (5000 个时钟周期，假设1个时钟周期 = 1毫秒)
    }
    // 如果任务显示后应退出:
    // printf("OLED 显示完成。任务将退出。\n");
}

/**
 * @brief OLED Hello World 应用的入口点。
 *
 * OHOS 系统调用此函数以启动应用功能。
 * 它创建一个新线程来运行 OLED 显示逻辑。
 */
static void OledHelloWorldAppEntry(void) {
    osThreadAttr_t threadAttr;

    threadAttr.name = "OledDisplayTask"; // 线程名称
    threadAttr.attr_bits = 0U;
    threadAttr.cb_mem = NULL;
    threadAttr.cb_size = 0U;
    threadAttr.stack_mem = NULL;
    threadAttr.stack_size = APP_THREAD_STACK_SIZE; // 定义的堆栈大小
    threadAttr.priority = APP_THREAD_PRIORITY;   // 定义的优先级

    if (osThreadNew((osThreadFunc_t)OledDisplayTask, NULL, &threadAttr) == NULL) {
        printf("[OledHelloWorldAppEntry] 致命错误：创建 OledDisplayTask 失败!\n");
    } else {
        printf("[OledHelloWorldAppEntry] OledDisplayTask 创建成功。\n");
    }
}

// 向 OHOS 系统注册应用入口函数。
// 系统将在启动此功能时调用 OledHelloWorldAppEntry。
APP_FEATURE_INIT(OledHelloWorldAppEntry);

