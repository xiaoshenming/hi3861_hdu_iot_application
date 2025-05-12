#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "hi_io.h"
#include "hi_i2c.h"
#include "iot_gpio.h"
#include "iot_i2c.h"

// --- 配置 ---
#define OLED_I2C_IDX HI_I2C_IDX_0 // I2C 总线 ID (Hi3861 通常是 0)
#define OLED_I2C_ADDR 0x3C       // 很多 128x64 OLED 的标准 I2C 地址 (0x78 >> 1)
#define OLED_I2C_BAUDRATE 400000 // I2C 波特率 400kHz

// Hi3861 上 I2C0 的 GPIO 引脚定义
#define I2C0_SDA_PIN HI_IO_NAME_GPIO_13      // I2C0 SDA 引脚
#define I2C0_SCL_PIN HI_IO_NAME_GPIO_14      // I2C0 SCL 引脚
#define I2C0_SDA_FUNC HI_IO_FUNC_GPIO_13_I2C0_SDA // I2C0 SDA 引脚功能
#define I2C0_SCL_FUNC HI_IO_FUNC_GPIO_14_I2C0_SCL // I2C0 SCL 引脚功能

// OLED 控制器命令 (SSD1306)
#define OLED_CMD 0x00  // 命令模式
#define OLED_DATA 0x40 // 数据模式

// SSD1306 基本初始化命令序列
static const uint8_t oled_init_cmds[] = {
    0xAE, // 关闭显示
    0xD5, // 设置显示时钟分频比/振荡器频率
    0x80, // 建议的比率 (0x80)
    0xA8, // 设置 MUX 比率
    0x3F, // 1/64 驱动占空比
    0xD3, // 设置显示偏移
    0x00, // 无偏移
    0x40, // 设置起始行地址 (0x40 = 第 0 行)
    0x8D, // 电荷泵设置
    0x14, // 使能电荷泵
    0x20, // 设置内存寻址模式
    0x00, // 水平寻址模式
    0xA1, // 设置段重映射 (A0=正常, A1=重映射) -> 列地址 127 映射到 SEG0
    0xC8, // 设置 COM 输出扫描方向 (C0=正常, C8=重映射) -> 从 COM[N-1] 扫描到 COM0
    0xDA, // 设置 COM 引脚硬件配置
    0x12, // 可选的 COM 引脚配置, 禁用 COM 左右重映射
    0x81, // 设置对比度控制
    0xCF, // 默认对比度
    0xD9, // 设置预充电周期
    0xF1, // 预充电: 15 时钟, 放电: 1 时钟
    0xDB, // 设置 VCOMH 取消选择电平
    0x40, // VCOMH = ~0.77 * Vcc
    0xA4, // 全局显示开启 (A4=输出跟随 RAM, A5=输出忽略 RAM)
    0xA6, // 设置正常显示 (A6=正常, A7=反相)
    0xAF  // 打开显示
};

// 简单的 5x7 ASCII 字体 (仅包含 "Hello World!" 所需字符)
// 注意: 这里的顺序对于下面的 switch 语句很重要
static const uint8_t font5x7[][5] = {
    // 索引: 字符
    {0x00, 0x00, 0x00, 0x00, 0x00}, // 0: 空格
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // 1: !
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // 2: H
    {0x7E, 0x11, 0x11, 0x11, 0x7E}, // 3: W  <-- 已修正 'W' 的字模数据
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // 4: d
    {0x7C, 0x08, 0x04, 0x04, 0x78}, // 5: e
    {0x00, 0x41, 0x7F, 0x40, 0x00}, // 6: l
    {0x3E, 0x41, 0x41, 0x22, 0x1C}, // 7: o
    {0x7C, 0x08, 0x04, 0x04, 0x08}  // 8: r
    // 如果需要，在此处添加其他字符并更新 OledShowChar 中的 switch 语句
};

// --- I2C 辅助函数 ---

/**
 * @brief 通过 I2C 发送数据。
 * @param data 指向数据缓冲区的指针。
 * @param len 要发送的数据长度。
 * @return 成功返回 0，失败返回非零值。
 */
static uint32_t OledI2cWrite(const uint8_t *data, uint32_t len)
{
    // 如果需要，可以分块发送数据，I2C 缓冲区可能有限
    // 为简单起见，这里一次性发送所有数据。
    return IoTI2cWrite(OLED_I2C_IDX, (OLED_I2C_ADDR << 1) | 0, data, len);
}

/**
 * @brief 向 OLED 发送命令。
 * @param cmd 命令字节。
 */
static void OledWriteCmd(uint8_t cmd)
{
    uint8_t buffer[2] = {OLED_CMD, cmd};
    uint32_t retval = OledI2cWrite(buffer, sizeof(buffer));
    if (retval != 0) {
        printf("错误：I2C 写命令失败！错误码：%u\n", retval);
    }
    // 如果遇到问题，可以在命令之间添加短暂延时
    // usleep(10); // 10 微秒
}

/**
 * @brief 向 OLED RAM 发送数据。
 * @param data 数据字节。
 */
static void OledWriteData(uint8_t data)
{
    uint8_t buffer[2] = {OLED_DATA, data};
    uint32_t retval = OledI2cWrite(buffer, sizeof(buffer));
    if (retval != 0) {
        printf("错误：I2C 写数据失败！错误码：%u\n", retval);
    }
    // 如果需要，可以添加短暂延时
    // usleep(10);
}

// --- OLED 控制函数 ---

/**
 * @brief 初始化 OLED 显示屏。
 */
static void OledInit(void)
{
    // 初始化 I2C 的 GPIO 引脚
    IoTGpioInit(I2C0_SDA_PIN);
    IoTGpioInit(I2C0_SCL_PIN);
    hi_io_set_func(I2C0_SDA_PIN, I2C0_SDA_FUNC);
    hi_io_set_func(I2C0_SCL_PIN, I2C0_SCL_FUNC);

    // 初始化 I2C 外设
    uint32_t retval = IoTI2cInit(OLED_I2C_IDX, OLED_I2C_BAUDRATE);
    if (retval != 0) {
        printf("错误：I2C 初始化失败！错误码：%u\n", retval);
        return;
    }
    printf("I2C 初始化成功\n");

    // 发送命令前短暂延时
    usleep(20000); // 20 毫秒

    // 发送初始化命令
    for (uint32_t i = 0; i < sizeof(oled_init_cmds); i++) {
        OledWriteCmd(oled_init_cmds[i]);
    }
    printf("OLED 初始化命令已发送\n");

    // 初始化序列后的额外延时
    usleep(100000); // 初始化后延时 100 毫秒
}

/**
 * @brief 清除 OLED 屏幕 RAM。
 */
static void OledClear(void)
{
    for (uint8_t page = 0; page < 8; page++) { // 8 页 (行)
        OledWriteCmd(0xB0 + page); // 设置页地址
        OledWriteCmd(0x00);       // 设置列地址低位起始为 0
        OledWriteCmd(0x10);       // 设置列地址高位起始为 0
        for (uint8_t col = 0; col < 128; col++) { // 128 列
            OledWriteData(0x00); // 写入 0 以清除
        }
    }
    printf("OLED 已清屏\n");
}

/**
 * @brief 在 OLED 上设置光标位置。
 * @param page 页码 (0-7)。
 * @param col 列号 (0-127)。
 */
static void OledSetPosition(uint8_t page, uint8_t col)
{
    if (page > 7) page = 7;
    if (col > 127) col = 127;
    OledWriteCmd(0xB0 + page);         // 设置页地址 (0-7)
    OledWriteCmd(0x00 | (col & 0x0F)); // 设置列地址的低 4 位
    OledWriteCmd(0x10 | (col >> 4));   // 设置列地址的高 4 位
}

/**
 * @brief 在当前位置显示一个字符。
 * @param c 要显示的字符。
 */
static void OledShowChar(char c)
{
    uint8_t font_index;

    // 将字符映射到我们有限的 font5x7 数组中的正确索引
    switch(c) {
        case ' ': font_index = 0; break;
        case '!': font_index = 1; break;
        case 'H': font_index = 2; break;
        case 'W': font_index = 3; break; // 使用修正后的 'W'
        case 'd': font_index = 4; break;
        case 'e': font_index = 5; break;
        case 'l': font_index = 6; break;
        case 'o': font_index = 7; break;
        case 'r': font_index = 8; break;
        default:  font_index = 0; break; // 对于未知字符，默认为空格
    }

    // 检查计算出的索引对于数组边界是否有效
    if (font_index >= (sizeof(font5x7) / sizeof(font5x7[0]))) {
        printf("错误：字符 '%c' 的字体索引 %d 超出边界\n", c, font_index);
        font_index = 0; // 回退到空格
    }

    const uint8_t *font_data = font5x7[font_index];

    // 写入 5 列的字体数据
    for (uint8_t i = 0; i < 5; i++) { // 字体宽度 = 5
        OledWriteData(font_data[i]);
    }
    // 通过写入一个空列来在字符之间添加 1 像素的间隙
    OledWriteData(0x00);
}

/**
 * @brief 在指定位置显示一个字符串。
 * @param page 页码 (0-7)。
 * @param col 起始列号 (0-127)。
 * @param str 要显示的以 null 结尾的字符串。
 */
static void OledShowString(uint8_t page, uint8_t col, const char *str)
{
    OledSetPosition(page, col);
    while (*str) {
        // 计算此字符所需的剩余列数 (5 宽度 + 1 间隙 = 6)
        if (col > 127 - 6) break; // 如果字符放不下则停止

        OledShowChar(*str++);
        col += 6; // 前进列位置
        // 注意: 这个简单版本不处理换行到下一行的情况。
    }
}

// --- 任务函数 ---

/**
 * @brief 初始化并在 OLED 上显示的主任务。
 */
void OledTask(void *arg)
{
    (void)arg; // 未使用的参数

    printf("OLED 任务已启动\n");

    // 初始化 OLED
    OledInit();

    // 初始化 *之后* 清屏
    OledClear();
    usleep(10000); // 清屏后短暂延时

    // 显示 "Hello World!"
    // 参数: 页 (0-7), 列 (0-127), 字符串
    OledShowString(3, 20, "Hello World!"); // 大致在垂直和水平方向居中显示
    printf("已在 OLED 上显示 'Hello World!'\n");

    // 任务完成其主要工作，可以循环或退出
    while (1) {
        osDelay(2000); // 保持任务活动，延时 2 秒
    }
}

// --- 应用程序入口 ---

/**
 * @brief 主应用程序入口点。
 */
static void OledHelloWorldEntry(void)
{
    osThreadAttr_t attr;

    printf("启动 OLED Hello World 示例\n");

    memset(&attr, 0, sizeof(attr));
    attr.name = "OledTask"; // 任务名称
    attr.stack_size = 4096; // 分配足够的栈空间
    attr.priority = osPriorityNormal; // 设置合适的优先级

    if (osThreadNew(OledTask, NULL, &attr) == NULL) {
        printf("错误：创建 OledTask 失败！\n");
    } else {
        printf("OledTask 创建成功\n");
    }
}

// 注册应用程序入口点
SYS_RUN(OledHelloWorldEntry);
