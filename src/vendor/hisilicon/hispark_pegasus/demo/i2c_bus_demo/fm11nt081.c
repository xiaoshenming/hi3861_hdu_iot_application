#include <stdio.h>
#include <unistd.h>

#include "fm11nt081.h"
#include "iot_gpio_ex.h"
#include "iot_gpio.h"
#include "hi_i2c.h"
#include "iot_i2c.h"
#include "pca9555.h"
#include "iot_errno.h"
#include "hi_errno.h"

static unsigned int C081NfcI2cWrite(unsigned char regHigh8bitCmd,
    unsigned char regLow8bitCmd, unsigned char* recvData, unsigned char sendLen)
{
    unsigned int status =0;
    IotI2cData c081NfcI2cWriteCmdAddr ={0};
    unsigned char sendUserCmd[64] = {regHigh8bitCmd, regLow8bitCmd};

    c081NfcI2cWriteCmdAddr.sendBuf = sendUserCmd;
    c081NfcI2cWriteCmdAddr.sendLen = 2 + sendLen;
    for (unsigned int i=0; i < sendLen; i++) {
        sendUserCmd[2+i] = *(recvData + i);
    }
    status = IoTI2cWrite(PCA9555_I2C_IDX, C081_NFC_ADDR & 0xFE,
        c081NfcI2cWriteCmdAddr.sendBuf, c081NfcI2cWriteCmdAddr.sendLen);
    if (status != IOT_SUCCESS) {
        printf("I2cWrite(%02X) failed, %0X!\n", sendUserCmd[0], status);
        return status;
    }
    return IOT_SUCCESS;
}

/*8
	@berf EEPROM page write
	@param unsigned char *pBuffer: Send data buff
	@param unsigned short WriteAddr:Send register address
	@param unsigned char datalen:Sending data length
*/
hi_void EepWritePage(unsigned char *pBuffer, unsigned short WriteAddr, unsigned char datalen)
{
    unsigned int status;
    if (pBuffer == NULL) {
        printf("eepWritePage buffer is null\r\n");
    }
    status = C081NfcI2cWrite((unsigned char)((WriteAddr & 0xFF00) >> 8),
        (unsigned char)(WriteAddr & 0x00FF), pBuffer, datalen);
    hi_udelay(10000); //The delay time must be 10ms
}

/**
    @berf write EEPROM
    @param unsigned short addr:eeprom address
    @param unsigned int len:Write data length
    @param unsigned char *wbuf:write data buff
*/
hi_void Fm11nt081dWriteEeprom(unsigned short baseAddr, unsigned int len, unsigned char *wbuf)
{
	unsigned char offset = 0;
    unsigned char *writeBuff = wbuf;
    unsigned int writeLen = len;
    unsigned short addr = baseAddr;
    if (writeBuff == NULL) {
        printf("write ndef is null\r\n");
        return;
    }
    if (addr < FM11_E2_USER_ADDR || addr >= FM11_E2_MAUNF_ADDR) {
        offset = FM11_E2_BLOCK_SIZE - (addr % FM11_E2_BLOCK_SIZE);
        printf("offset = %d, writeLen = %d\r\n", offset, writeLen);
        if (writeLen > offset) {
            EepWritePage(writeBuff, addr, offset);
            addr += offset;
            writeBuff += offset;
            writeLen -= offset;
        } else {
            EepWritePage(writeBuff, addr, writeLen);
            writeLen = 0;
        }
    }
    while (writeLen) {
        if (writeLen >= FM11_E2_BLOCK_SIZE) {
            EepWritePage(writeBuff, addr, FM11_E2_BLOCK_SIZE);
            addr += FM11_E2_BLOCK_SIZE;
            writeBuff += FM11_E2_BLOCK_SIZE;
            writeLen -= FM11_E2_BLOCK_SIZE;
        } else {
            EepWritePage(writeBuff, addr, writeLen);
            writeLen = 0;
        }
    }
}

/**
 * @berf i2c read
 * @param unsigned char reg_high_8bit_cmd:Transmit register value 8 bits high
 * @param unsigned char reg_low_8bit_cmd:Transmit register value low 8 bits
 * @param unsigned char* recv_data:Receive data buff
 * @param unsigned char send_len:Sending data length
 * @param unsigned char read_len:Length of received data
*/
unsigned int WriteRead(unsigned char regHigh8bitCmd, unsigned char regLow8bitCmd,
    unsigned char sendLen, unsigned char readLen)
{
    unsigned int status = 0;
    unsigned char recvData[888] = {0};
    hi_i2c_data c081NfcI2cWriteCmdAddr ={0};
    unsigned char sendUserCmd[2] = {regHigh8bitCmd, regLow8bitCmd};
    (void)memset_s(&recvData, sizeof(unsigned char), 0x0, sizeof(recvData));

    c081NfcI2cWriteCmdAddr.send_buf = sendUserCmd;
    c081NfcI2cWriteCmdAddr.send_len = sendLen;

    c081NfcI2cWriteCmdAddr.receive_buf = recvData;
    c081NfcI2cWriteCmdAddr.receive_len = readLen;

    status = hi_i2c_writeread(PCA9555_I2C_IDX, C081_NFC_ADDR | I2C_RD, &c081NfcI2cWriteCmdAddr);
    if (status != IOT_SUCCESS) {
        printf("hi_i2c_writeread failed, %0X!\n", status);
        return status;
    }

    return IOT_SUCCESS;
}

/**
 * @berf read EEPROM
 * @param unsigned char *dataBuff:Read data and save it in buff
 * @param unsigned short ReadAddr:Read address
 * @param unsigned short len:Read length
*/
unsigned int Fm11nt081ReadEep(unsigned short ReadAddr, unsigned short len)
{
    unsigned int status;
    status = WriteRead((unsigned char)((ReadAddr & 0xFF00)>>8), (unsigned char)(ReadAddr & 0x00FF), 2, len);
    return  IOT_SUCCESS;
}

/*NFC chip configuration, usually do not call NFC init*/
void NFCInit(void)
{
    unsigned char wbuf[5] = {0x05, 0x78, 0xF7, 0x90, 0x02};   //Chip default configuration
    /*The CSN pin is masked when the byte is read and turned on when the EEP is written*/
    hi_udelay(100);
    Fm11nt081dWriteEeprom(0x3B1, 1, &wbuf[1]);
    Fm11nt081dWriteEeprom(0x3B5, 1, &wbuf[3]);
    Fm11nt081dWriteEeprom(NFC_EERROM_BASE_ADD, NFC_TOUTIAO_NDEF_LEN, ndefFile);
    Fm11nt081ReadEep(NFC_EERROM_BASE_ADD, READ_NFC_TOUTIAO_NDEF_LEN);
}