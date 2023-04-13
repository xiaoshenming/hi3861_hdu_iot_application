/*
 * Copyright (c) 2023 Beijing HuaQing YuanJian Education Technology Co., LTD
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "hal_bsp_ssd1306.h"
#include "hal_bsp_ssd1306_fonts.h"
#include "hi_errno.h"
#include "hi_i2c.h"
#include "hi_io.h"
#include "hi_gpio.h"

#define RESET_TIME (100 * 1000) // 100ms

static uint32_t SSD1306_SendData(uint8_t *data, size_t size)
{
    hi_i2c_data i2cData = {0};
    i2cData.send_buf = data;
    i2cData.send_len = size;

    return hi_i2c_write(SSD1306_I2C_IDX, SSD1306_I2C_ADDR, &i2cData);
}

// 写命令
static uint32_t SSD1306_WriteCmd(uint8_t byte)
{
    uint8_t buffer[] = {0x00, byte};
    return SSD1306_SendData(buffer, sizeof(buffer));
}
// 写数据
static uint32_t SSD1306_WiteData(uint8_t byte)
{
    uint8_t buffer[] = {0x40, byte};
    return SSD1306_SendData(buffer, sizeof(buffer));
}

uint32_t SSD1306_Init(void)
{
    uint32_t result;

    // gpio_9 复用为 I2C_SCL
    hi_io_set_pull(HI_IO_NAME_GPIO_9, HI_IO_PULL_UP);
    hi_io_set_func(HI_IO_NAME_GPIO_9, HI_IO_FUNC_GPIO_9_I2C0_SCL);
    // gpio_10 复用为 I2C_SDA
    hi_io_set_pull(HI_IO_NAME_GPIO_10, HI_IO_PULL_UP);
    hi_io_set_func(HI_IO_NAME_GPIO_10, HI_IO_FUNC_GPIO_10_I2C0_SDA);

    result = hi_i2c_init(SSD1306_I2C_IDX, SSD1306_I2C_SPEED);
    if (result != HI_ERR_SUCCESS) {
        printf("I2C SSD1306 Init status is 0x%x!!!\r\n", result);
        return result;
    }

    usleep(RESET_TIME);

    SSD1306_WriteCmd(0xAE); // display off
    SSD1306_WriteCmd(0x20); // Set Memory Addressing Mode
    SSD1306_WriteCmd(0x10); // 10,Page Addressing Mode (RESET)
    SSD1306_WriteCmd(0xb0); // Set Page Start Address for Page Addressing Mode,0-7
    SSD1306_WriteCmd(0xc8); // Set COM Output Scan Direction
    SSD1306_WriteCmd(0x00); // ---set low column address
    SSD1306_WriteCmd(0x10); // ---set high column address
    SSD1306_WriteCmd(0x40); // --set start line address
    SSD1306_WriteCmd(0x81); // --set contrast control register
    SSD1306_WriteCmd(0xff); //  亮度调节 0x00~0xff
    SSD1306_WriteCmd(0xa1); // --set segment re-map 0 to 127
    SSD1306_WriteCmd(0xa6); // --set normal display
    SSD1306_WriteCmd(0xa8); // --set multiplex ratio(1 to 64)
    SSD1306_WriteCmd(0x3F); //
    SSD1306_WriteCmd(0xa4); // 0xa4,Output follows RAM content;0xa5,Output ignores RAM content
    SSD1306_WriteCmd(0xd3); // -set display offset
    SSD1306_WriteCmd(0x00); // -not offset
    SSD1306_WriteCmd(0xd5); // --set display clock divide ratio/oscillator frequency
    SSD1306_WriteCmd(0xf0); // --set divide ratio
    SSD1306_WriteCmd(0xd9); // --set pre-charge period
    SSD1306_WriteCmd(0x22); //
    SSD1306_WriteCmd(0xda); // --set com pins hardware configuration
    SSD1306_WriteCmd(0x12);
    SSD1306_WriteCmd(0xdb); // --set vcomh
    SSD1306_WriteCmd(0x20); //  0x20,0.77xVcc
    SSD1306_WriteCmd(0x8d); // --set DC-DC enable
    SSD1306_WriteCmd(0x14); //
    SSD1306_WriteCmd(0xaf); // --turn on oled panel
    SSD1306_WriteCmd(0xAF); // display ON
    SSD1306_SetPos(0, 0);

    printf("I2C SSD1306 Init is succeeded!!!\r\n");
    return HI_ERR_SUCCESS;
}
/**
 * @brief  垂直滚动函数
 * @retval None
 */
void OLED_Set_Vertical_Rol(void)
{
    SSD1306_WriteCmd(0x2e); // 停止滚动
    SSD1306_WriteCmd(0xa3); //
    SSD1306_WriteCmd(0x00);
    SSD1306_WriteCmd(0x40);
    SSD1306_WriteCmd(0x2a);
    SSD1306_WriteCmd(0x00);
    SSD1306_WriteCmd(0x00);
    SSD1306_WriteCmd(0x01);
    SSD1306_WriteCmd(0x00);
    SSD1306_WriteCmd(0x04);
    SSD1306_WriteCmd(0x2f);
}

void SSD1306_SetPos(unsigned char x, unsigned char y) // 设置起始点坐标
{
    uint8_t illegal_keyword = 4;  // 右移4位
    SSD1306_WriteCmd(0xb0 + y);
    SSD1306_WriteCmd(((x & 0xf0) >> illegal_keyword) | 0x10);
    SSD1306_WriteCmd((x & 0x0f) | 0x01);
}

void SSD1306_Fill(unsigned char fill_Data) // 全屏填充
{
    unsigned char m, n;
    uint8_t number_of_rows = 8;
    uint8_t number_of_columns = 128;

    for (m = 0; m < number_of_rows; m++) {
        SSD1306_WriteCmd(0xb0 + m); // page0-page1
        SSD1306_WriteCmd(0x00);     // low column start address
        SSD1306_WriteCmd(0x10);     // high column start address
        for (n = 0; n < number_of_columns; n++) {
            SSD1306_WiteData(fill_Data);
        }
    }
}

void SSD1306_CLS(void) // 清屏
{
    SSD1306_Fill(0x00);
}

// --------------------------------------------------------------
// Prototype      : void OLED_ON(void)
// Parameters     : none
// Description    : 将OLED从休眠中唤醒
// --------------------------------------------------------------
void SSD1306_ON(void)
{
    SSD1306_WriteCmd(0X8D); // 设置电荷泵
    SSD1306_WriteCmd(0X14); // 开启电荷泵
    SSD1306_WriteCmd(0XAF); // OLED唤醒
}

// --------------------------------------------------------------
// Prototype      : void OLED_OFF(void)
// Parameters     : none
// Description    : 让OLED休眠 -- 休眠模式下,OLED功耗不到10uA
// --------------------------------------------------------------
void SSD1306_OFF(void)
{
    SSD1306_WriteCmd(0X8D); // 设置电荷泵
    SSD1306_WriteCmd(0X10); // 关闭电荷泵
    SSD1306_WriteCmd(0XAE); // OLED休眠
}

/**
 * @brief  显示字符串
 */
void SSD1306_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize)
{
    unsigned char c = 0, i = 0, j = 0;
    uint8_t maximum_row_position = 126;
    uint8_t number_of_letter_lattice = 6;
    uint8_t data_num = 8;  // 一个字节8位
    uint8_t pos_x = x;
    uint8_t pos_y = y;

    switch (TextSize) {
        case TEXT_SIZE_8:
            while (ch[j] != '\0') {
                c = ch[j] - ' ';
                if (pos_x > maximum_row_position) {
                    pos_x = 0;
                    pos_y++;
                }
                SSD1306_SetPos(pos_x, pos_y);
                for (i = 0; i < number_of_letter_lattice; i++) {
                    SSD1306_WiteData(F6x8[c][i]);
                }
                pos_x += 6;
                j++;
            }
            break;

        case TEXT_SIZE_16:
            pos_y *= 2;
            while (ch[j] != '\0') {
                c = ch[j] - ' ';
                if (pos_x > 120) {
                    pos_x = 0;
                    pos_y++;
                }
                SSD1306_SetPos(pos_x, pos_y);
                for (i = 0; i < data_num; i++) {
                    SSD1306_WiteData(F8X16[c * 16 + i]);
                }
                SSD1306_SetPos(pos_x, pos_y + 1);

                for (i = 0; i < data_num; i++) {
                    SSD1306_WiteData(F8X16[c * 16 + i + 8]);
                }
                pos_x += data_num;
                j++;
            }
            break;

        default:
            break;
    }
}

/**
 * @brief  显示一张图片
 * @note   取模方式： 阳码、列行式、逆向、十六进制数、输出索引格、C51格式
 * @param  xMove: x坐标
 * @param  yMove: y坐标
 * @param  width: 宽
 * @param  height: 高
 */
void SSD1306_DrawBMP(unsigned char xMove, unsigned char yMove,
                    unsigned char width, unsigned char height,
                    unsigned char *BMP)
{
    unsigned short j = 0, x = 0, y = 0;
    unsigned char x0 = xMove;
    unsigned char y0 = yMove / 8;
    unsigned char x1 = xMove + width;
    unsigned char y1 = height / 8 + y0;

    for (y = y0; y < y1; y++) {
        SSD1306_SetPos(x0, y);
        for (x = x0; x < x1; x++) {
            SSD1306_WiteData(BMP[j]);
            j++;
        }
    }
}