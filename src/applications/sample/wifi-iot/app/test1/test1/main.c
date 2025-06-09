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

#include "ssd1306.h"        // 核心 SSD1306 驱动函数

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

// --- 中文字符点阵数据 (16x16) ---
// 数据来源提示：汉字字模在线工具 https://www.23bei.com/tool-223.html
// 数据排列：从左到右从上到下，取模方式：横向8位左高位 (每行2字节)

// 定义字模的宽度和高度 (以像素为单位)
#define CHINESE_CHAR_WIDTH  16
#define CHINESE_CHAR_HEIGHT 16
// 每个字符的点阵数据字节数 (宽度像素/8 * 高度像素)
#define CHINESE_CHAR_BYTES  ((CHINESE_CHAR_WIDTH / 8) * CHINESE_CHAR_HEIGHT) // (16/8 * 16 = 2 * 16 = 32 bytes)

static const uint8_t chinese_fonts[][CHINESE_CHAR_BYTES] = {
    { /* -- ID:0,字符:"你",ASCII编码:C4E3,对应字:宽x高=16x16,画布:宽W=16 高H=16,共32字节 -- */
      0x11, 0x00, 0x11, 0x00, 0x11, 0x00, 0x23, 0xFC, 0x22, 0x04, 0x64, 0x08, 0xA8, 0x40, 0x20, 0x40,
      0x21, 0x50, 0x21, 0x48, 0x22, 0x4C, 0x24, 0x44, 0x20, 0x40, 0x20, 0x40, 0x21, 0x40, 0x20, 0x80,
    }, { /* -- ID:1,字符:"好",ASCII编码:BAC3,对应字:宽x高=16x16,画布:宽W=16 高H=16,共32字节 -- */
      0x10, 0x00, 0x11, 0xFC, 0x10, 0x04, 0x10, 0x08, 0xFC, 0x10, 0x24, 0x20, 0x24, 0x24, 0x27, 0xFE,
      0x24, 0x20, 0x44, 0x20, 0x28, 0x20, 0x10, 0x20, 0x28, 0x20, 0x44, 0x20, 0x84, 0xA0, 0x00, 0x40,
    }, { /* -- ID:2,字符:"鸿",ASCII编码:BAE8,对应字:宽x高=16x16,画布:宽W=16 高H=16,共32字节 -- */
      0x40, 0x20, 0x30, 0x48, 0x10, 0xFC, 0x02, 0x88, 0x9F, 0xA8, 0x64, 0x88, 0x24, 0xA8, 0x04, 0x90,
      0x14, 0x84, 0x14, 0xFE, 0xE7, 0x04, 0x3C, 0x24, 0x29, 0xF4, 0x20, 0x04, 0x20, 0x14, 0x20, 0x08,
    }, { /* -- ID:3,字符:"蒙",ASCII编码:C3C9,对应字:宽x高=16x16,画布:宽W=16 高H=16,共32字节 -- */
      0x04, 0x48, 0x7F, 0xFC, 0x04, 0x40, 0x7F, 0xFE, 0x40, 0x02, 0x8F, 0xE4, 0x00, 0x00, 0x7F, 0xFC,
      0x06, 0x10, 0x3B, 0x30, 0x05, 0xC0, 0x1A, 0xA0, 0x64, 0x90, 0x18, 0x8E, 0x62, 0x84, 0x01, 0x00,
    }
};
// --- 中文字符点阵数据结束 ---


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
    ssd1306_Init(); 
    printf("SSD1306 OLED 初始化序列已启动。\n");

    // 初始化后清屏是一个好习惯
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen(); // 确保清屏命令已发送

    return 0; // 如果 I2C 和 GPIO 初始化通过，则返回成功。
}

/**
 * @brief OLED 显示应用的主任务。
 *
 * 此任务初始化 OLED，清屏，然后写入指定的字符串和图像。
 * @param arg 任务参数 (未使用)。
 */
static void OledDisplayTask(void *arg) {
    (void)arg; // 标记参数为未使用
    uint8_t current_y = 0; // 用于跟踪当前Y轴绘制位置
    const uint8_t line_padding = 2; // 行间距

    printf("OledDisplayTask 已启动。\n");

    // 初始化 OLED 和外设
    if (OledPeripheralInit() != 0) {
        printf("OLED 外设初始化失败。正在退出任务。\n");
        return;
    }

    // --- 1. 显示 "hello world" ---
    ssd1306_SetCursor(0, current_y); 
    ssd1306_DrawString("hello world", Font_7x10, White);
    printf("尝试绘制 'hello world'。\n");
    current_y += Font_7x10.FontHeight + line_padding; // Font_7x10.FontHeight 通常是 10

    // --- 2. 显示 "你好鸿蒙" (使用16x16自定义点阵) ---
    // 确保 current_y 有足够空间给16像素高的字符
    if (current_y + CHINESE_CHAR_HEIGHT > SSD1306_HEIGHT) {
        printf("屏幕空间不足以绘制中文。\n");
    } else {
        uint8_t current_x = 0;
        for (size_t i = 0; i < sizeof(chinese_fonts) / sizeof(chinese_fonts[0]); i++) {
            if (current_x + CHINESE_CHAR_WIDTH <= SSD1306_WIDTH) {
                 // ssd1306_DrawRegion(x_target, y_target, width_to_draw, height_to_draw, bitmap_data, bitmap_data_total_size, bitmap_stride_pixels);
                 // 对于 ssd1306_DrawBitmap (如果可用且更简单): ssd1306_DrawBitmap(current_x, current_y, chinese_fonts[i], CHINESE_CHAR_WIDTH, CHINESE_CHAR_HEIGHT, White);
                 // 使用 ssd1306_DrawRegion，基于提供的示例：
                 // 参数: x, y, region_width, region_height, data_ptr, data_len, stride_in_pixels
                 // stride_in_pixels 是源点阵数据的每行像素数
                ssd1306_DrawRegion(current_x, current_y, CHINESE_CHAR_WIDTH, CHINESE_CHAR_HEIGHT, 
                                   chinese_fonts[i], CHINESE_CHAR_BYTES, CHINESE_CHAR_WIDTH);
                current_x += CHINESE_CHAR_WIDTH; // 每个中文字符宽度为16像素
            } else {
                printf("行空间不足以绘制所有中文字符。\n");
                break; 
            }
        }
        printf("尝试绘制 '你好鸿蒙' (自定义点阵)。\n");
        current_y += CHINESE_CHAR_HEIGHT + line_padding;
    }
    
    // --- 3. 显示 "Status: OK" ---
    if (current_y + Font_7x10.FontHeight <= SSD1306_HEIGHT) {
        ssd1306_SetCursor(0, current_y);
        ssd1306_DrawString("Status: OK", Font_7x10, White);
        printf("尝试绘制 'Status: OK'。\n");
        current_y += Font_7x10.FontHeight + line_padding;
    }

    // --- 4. 显示 "Temp: 25.5C" ---
     if (current_y + Font_7x10.FontHeight <= SSD1306_HEIGHT) {
        ssd1306_SetCursor(0, current_y);
        ssd1306_DrawString("Temp: 25.5C", Font_7x10, White);
        printf("尝试绘制 'Temp: 25.5C'。\n");
        current_y += Font_7x10.FontHeight + line_padding;
    }

    // --- 5. 显示 "IP: 192.168.1.1" ---
    if (current_y + Font_7x10.FontHeight <= SSD1306_HEIGHT) {
        ssd1306_SetCursor(0, current_y);
        // 注意: Font_7x10 可能无法完整显示长IP地址，需要测试或使用更小字体/滚动
        ssd1306_DrawString("IP:192.168.1.1", Font_7x10, White); 
        printf("尝试绘制 'IP:192.168.1.1'。\n");
        // current_y += Font_7x10.FontHeight + line_padding; // 最后一行，不需要再增加 current_y
    }
    
    // 将缓冲区内容传输到 OLED 屏幕
    ssd1306_UpdateScreen();
    printf("OLED 屏幕已更新。请检查显示。\n");

    int count = 0;
    while (1) {
        // printf("OLED 显示任务存活... 循环 %d\n", ++count); // 可以注释掉以减少串口输出
        osDelay(5000); // 延迟5秒
    }
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

