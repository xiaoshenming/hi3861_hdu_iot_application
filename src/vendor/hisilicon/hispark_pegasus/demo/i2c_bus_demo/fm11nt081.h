/*
    FM11NT081DT NFC模块的相关API接口
*/
#ifndef __FM11NT081_H__
#define __FM11NT081_H__

#include <stdint.h>

#define NFC_EERROM_BASE_ADD              (0x0010)
#define NFC_EERROM_FINALY_ADDR           (0x0384)
#define NFC_CMD_LEN                      (2)
#define NFC_WECHAT_NDEF_LEN              (34)
#define READ_NFC_WECHAT_NDEF_LEN         (50)
#define NFC_TOUTIAO_NDEF_LEN             (47)
#define READ_NFC_TOUTIAO_NDEF_LEN        (63)
#define NFC_EERROM_READ_BUFF_LEN_MAX     (888)
#define C081_NFC_ADDR                    (0xAE)
#define I2C_WR                           (0x00)
#define I2C_RD                           (0x01)
#define C081_NFC_WRITE_ADDR              (C081_NFC_ADDR|I2C_WR)
#define C081_NFC_READ_ADDR               (C081_NFC_ADDR|I2C_RD)
#define FM11_E2_USER_ADDR                (0x0010)
#define FM11_E2_MAUNF_ADDR               (0x03FF)
#define FM11_E2_BLOCK_SIZE               (16)
#define DEFAULT_MD_LEN                   (128)
#define RIGHR_MOVE_8_BIT                 (8)
#define NFC_NDEF_MAX_LEN                 (888)
#define OLED_I2C_BAUDRATE                400*1000
#define C081_NFC_ADDR2                   (0x78)

unsigned char ndefFile[NFC_NDEF_MAX_LEN] = {
    // 淘宝
    0x03, 0x23,
    0xd4, 0x0f, 0x11, 0x61, 0x6e, 0x64, 0x72, 0x6f,
    0x69, 0x64, 0x2e, 0x63, 0x6f, 0x6d, 0x3a, 0x70,
    0x6b, 0x67, 0x63, 0x6f, 0x6d, 0x2e, 0x74, 0x61,
    0x6f, 0x62, 0x61, 0x6f, 0x2e, 0x74, 0x61, 0x6f,
    0x62, 0x61, 0x6f,
    // 微信
    /*0x03,0x20,
    0xd4, 0x0f,0x0e, 0x61, 0x6e, 0x64, 0x72, 0x6f,
    0x69, 0x64,0x2e, 0x63, 0x6f, 0x6d, 0x3a, 0x70,
    0x6b, 0x67,0x63, 0x6f, 0x6d, 0x2e, 0x74, 0x65,
    0x6e, 0x63,0x65, 0x6e, 0x74, 0x2e, 0x6d, 0x6d,*/
};

typedef struct {
    /* Pointer to the buffer storing data to send */
    unsigned char *sendBuf;
    /* Length of data to send */
    unsigned int sendLen;
    /* Pointer to the buffer for storing data to receive */
    unsigned char *receiveBuf;
    /* Length of data received */
    unsigned int receiveLen;
} IotI2cData;

// nfc 模块初始化
void NFCInit(void);

#endif // __FM11NT081_H__