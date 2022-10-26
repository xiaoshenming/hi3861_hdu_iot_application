#include "pca.h"
#include "iot_gpio_ex.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "iot_i2c.h"
#include "iot_errno.h"
#include "hi_errno.h"
#include "hi_i2c.h"
#include "iot_gpio.h"

#define PCA5555_I2C_IDX 0
#define IOT_I2C_IDX_BAUDRATE         400000 // 400k
#define	GET_BIT(x, bit)	((x & (1 << bit)) >> bit)
volatile static int g_ext_io_int_valid = 0;
//static uint8_t button[255] = { 0 };

/**
 * @berf i2c read
 * @param hi_u8 reg_high_8bit_cmd:Transmit register value 8 bits high
 * @param hi_u8 reg_low_8bit_cmd:Transmit register value low 8 bits
 * @param hi_u8* recv_data:Receive data buff
 * @param hi_u8 send_len:Sending data length
 * @param hi_u8 read_len:Length of received data
*/
static uint32_t PCA95555_WriteRead(uint8_t reg_high_8bit_cmd, uint8_t send_len, uint8_t read_len)
{
    uint32_t status =0;
    uint32_t ret = 0;
    uint8_t recvData[888] = {0};
    hi_i2c_data c081nfc_i2c_write_cmd_addr ={0};
    uint8_t send_user_cmd[1] = {reg_high_8bit_cmd};
    memset(recvData, 0x0, sizeof(recvData));
    c081nfc_i2c_write_cmd_addr.send_buf = send_user_cmd;
    c081nfc_i2c_write_cmd_addr.send_len = send_len;

    c081nfc_i2c_write_cmd_addr.receive_buf = recvData;
    c081nfc_i2c_write_cmd_addr.receive_len = read_len;

    status = hi_i2c_writeread(PCA5555_I2C_IDX, PCA9555_READ_ADDR, &c081nfc_i2c_write_cmd_addr);
    if (status != HI_ERR_SUCCESS) {
        return status;
    }
    ret = recvData[0];
    return ret;
}

static uint32_t PCA95555_Write(uint8_t* buffer, uint32_t buffLen)
{
    uint32_t retval = IoTI2cWrite(PCA5555_I2C_IDX, PCA9555_WRITE_ADDR, buffer, buffLen);
    if (retval != IOT_SUCCESS) {
        printf("IoTI2cWrite(%02X) failed, %0X!\n", buffer[0], retval);
        return retval;
    }
    return IOT_SUCCESS;
}

void PCA_Gpio_Config(uint8_t addr, uint8_t buffer, uint32_t buffLen)
{
    uint8_t write[WRITELEN] = {addr, buffer};
    PCA95555_Write(write, WRITELEN);
}


#define LEFT_LAMP_ON()      g_ext_io_output |= 0x38; PCA_Gpio_Config(PCA9555_REG_OUT1, g_ext_io_output, 2);
#define LEFT_LAMP_OFF()      g_ext_io_output &= (~0x38); PCA_Gpio_Config(PCA9555_REG_OUT1, g_ext_io_output, 2);
#define RIGHT_LAMP_ON()      g_ext_io_output |= 0x07; PCA_Gpio_Config(PCA9555_REG_OUT1, g_ext_io_output, 2);
#define RIGHT_LAMP_OFF()      g_ext_io_output &= (~0x07); PCA_Gpio_Config(PCA9555_REG_OUT1, g_ext_io_output, 2);

extern void car_forward();
extern void car_backward();

uint8_t g_ext_io_input = 0;
uint8_t g_ext_io_input_d = 0;
uint8_t g_ext_io_output = 0;
int g_intLowFlag = 0;
int32_t g_encoderLeftACounter = 0;
uint32_t g_StartTick = 0;
uint8_t g_car_on_off = 0;



void PCA9555_int_proc()
{
    uint8_t diff;

    if (g_ext_io_int_valid == 1) {
        g_ext_io_input = PCA95555_WriteRead(PCA9555_REG_IN0, 1, 1);
        diff = g_ext_io_input ^ g_ext_io_input_d;

        //printf("ext_io_input = %02x\r\n", ext_io_input);

        /* ext io 0 - 1: lighting sensor */
        if (diff & 0x01) {
            if (g_ext_io_input & 0x01) {
                printf("left lighten\n");
                LEFT_LAMP_ON();
            } else {
                printf("left darken\n");
                LEFT_LAMP_OFF();
            }
        }
        if (diff & 0x02) {
            if (g_ext_io_input & 0x02) {
                printf("right lighten\n");
                RIGHT_LAMP_ON();
            } else {
                printf("right darken\n");
                RIGHT_LAMP_OFF();
            }
        }

        /* ext io 2 - 4: user button */
        if ((diff & 0x04) && ((g_ext_io_input & 0x04) == 0)) {
            // button1 pressed event proc
            printf("button 1 pressed\n");
            car_left();
            g_car_on_off = 1;
        } else if ((diff & 0x08) && ((g_ext_io_input & 0x08) == 0)) {
            // button2 pressed event proc
            printf("button 2 pressed\n");
            car_right();
            g_car_on_off = 1;
        } else if ((diff & 0x10) && ((g_ext_io_input & 0x10) == 0)) {
            // button3 pressed event proc
            printf("button 3 pressed\n");
            g_car_on_off = !g_car_on_off;
            if(g_car_on_off)
            {
                car_forward();
            }
            else
            {
                car_stop();
            }
        }

        /* ext io 5 - 6: motor encoder */
        if ((diff & 0x20) && ((g_ext_io_input & 0x20) == 0)) {
            g_encoderLeftACounter++;
        }
        g_ext_io_int_valid = 0;
        g_ext_io_input_d = g_ext_io_input;
    }
    else 
    {
        IotGpioValue value = 0;
        if (IoTGpioGetInputVal(IOT_IO_NAME_GPIO_11, &value) == IOT_SUCCESS) {
            if (value == 1) {
                g_intLowFlag = 0;
            } else {
                if (g_intLowFlag == 0) {
                    g_StartTick = hi_get_milli_seconds();
                    g_intLowFlag = 1;
                } else {
                    if ((hi_get_milli_seconds() - g_StartTick) > 2) {
                        g_ext_io_input = PCA95555_WriteRead(PCA9555_REG_IN0, 1, 1);
                        g_intLowFlag = 0;
                    }
                }
            }
        }
    }
}


void PCA95555TestTask()
{
    uint8_t status;
    // PCA_Gpio_Config(PCA9555_REG_CFG0, 0x7F, 2); /* 输入为1，输出为0，IO0 234输入,0x60代表只用编码器，0x7c代表按键编码器同时使用，0x1c代表只用按键 */
    // PCA_Gpio_Config(PCA9555_REG_CFG1, 0x00, 2); /*IO1 012345输出 */
    // PCA_Gpio_Config(PCA9555_REG_OUT1, LED_OFF, 2); /*IO1 012345低电平 */
    // LSM6DSTask();
    // car_forward();

    /* PCA Input
        /* ext io 0 - 1: lighting sensor
        ext io 2 - 4: user button
        ext io 5 - 6: motor encoder    
     */

    PCA_Gpio_Config(PCA9555_REG_CFG0, 0x1F, 2);     /*  */
    PCA_Gpio_Config(PCA9555_REG_CFG1, 0x00, 2);     /*IO1 012345输出 */
    PCA_Gpio_Config(PCA9555_REG_OUT1, LED_OFF, 2);  /*IO1 012345低电平 */
    while (1) {
        PCA9555_int_proc();

        {
            static int time_stamp = 0;
            static int encoderLeftACounter_d = 0;
            if((hi_get_milli_seconds() - time_stamp) > 100)
            {
                //printf("rpm: %d\n", (g_encoderLeftACounter - encoderLeftACounter_d) * 60);
                encoderLeftACounter_d = g_encoderLeftACounter;
                time_stamp = hi_get_milli_seconds();

                if((g_ext_io_input & 0x03) == 0x02)
                {
                    car_left();
                }
                else if((g_ext_io_input & 0x03) == 0x01)
                {
                    car_right();
                }
                else if((g_ext_io_input & 0x03) == 0x03)
                {
                    car_forward();
                }
                else
                {
                    car_stop();
                }
            }
        }
    }
}

void OnExtIoTriggered(const char *arg)
{
    (void) arg;
    g_ext_io_int_valid = 1;
}

void PCA95555GpioInit()
{
    IoTI2cInit(0, IOT_I2C_IDX_BAUDRATE); /* baudrate: 400000 */
    IoTI2cSetBaudrate(0, IOT_I2C_IDX_BAUDRATE);
    IoSetFunc(IOT_IO_NAME_GPIO_13, IOT_IO_FUNC_GPIO_13_I2C0_SDA);
    IoSetFunc(IOT_IO_NAME_GPIO_14, IOT_IO_FUNC_GPIO_14_I2C0_SCL);

    IoTGpioInit(IOT_IO_NAME_GPIO_11);
    IoSetFunc(IOT_IO_NAME_GPIO_11, IOT_IO_FUNC_GPIO_11_GPIO);
    IoTGpioSetDir(IOT_IO_NAME_GPIO_11, IOT_GPIO_DIR_IN);
    IoSetPull(IOT_IO_NAME_GPIO_11, IOT_IO_PULL_UP);
    IoTGpioRegisterIsrFunc(IOT_IO_NAME_GPIO_11, IOT_INT_TYPE_EDGE, IOT_GPIO_EDGE_FALL_LEVEL_LOW, OnExtIoTriggered, NULL);
}

void TraceDemoTest(void)
{
    osThreadAttr_t attr;
    PCA95555GpioInit();
    GA12N20Init();
    IoTWatchDogDisable();
    attr.name = "PCA95555Task";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 5 * 1024;
    attr.priority = osPriorityNormal;

    if (osThreadNew(PCA95555TestTask, NULL, &attr) == NULL) {
        printf("[robot_car_demo] Failed to create Aht20TestTask!\n");
    }
}
APP_FEATURE_INIT(TraceDemoTest);