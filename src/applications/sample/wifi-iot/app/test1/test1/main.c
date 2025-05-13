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
#include <string.h> // For strlen, though not strictly necessary for ssd1306_DrawString

#include "ohos_init.h"      // For APP_FEATURE_INIT
#include "cmsis_os2.h"      // For osThreadAttr_t, osThreadNew, osPriorityNormal, osDelay
#include "iot_gpio.h"       // For IoTGpioInit
#include "iot_i2c.h"        // For IoTI2cInit
#include "iot_errno.h"      // For IOT_SUCCESS
#include "hi_io.h"          // For hi_io_set_func and GPIO function definitions

#include "ssd1306.h"        // Core SSD1306 driver functions

// --- Hardware Configuration ---
// IMPORTANT: Verify and update these GPIO and I2C settings for your specific board!
#define OLED_I2C_PORT     (0) // I2C Port (e.g., 0 or 1)
#define OLED_I2C_BAUDRATE (400 * 1000) // 400kHz, a common baud rate for SSD1306

// Example GPIO pins for I2C0 on a Hi3861-based board.
// Replace these with the correct pins for your hardware.
#define OLED_GPIO_SDA     HI_IO_NAME_GPIO_13
#define OLED_GPIO_SCL     HI_IO_NAME_GPIO_14

// Example GPIO pin function settings for I2C0.
// Replace these with the correct functions for your hardware and selected pins.
#define OLED_I2C_SDA_FUNC HI_IO_FUNC_GPIO_13_I2C0_SDA
#define OLED_I2C_SCL_FUNC HI_IO_FUNC_GPIO_14_I2C0_SCL
// --- End Hardware Configuration ---

#define APP_THREAD_STACK_SIZE (10240) // Stack size for the application task
#define APP_THREAD_PRIORITY   osPriorityNormal // Priority for the application task

/**
 * @brief Initializes GPIO, I2C, and the SSD1306 OLED display.
 *
 * @return 0 on success, -1 on failure (for peripheral init, not SSD1306_Init itself).
 */
static int OledPeripheralInit(void) {
    uint32_t ret;

    // Initialize GPIO pins for I2C
    if (IoTGpioInit(OLED_GPIO_SDA) != IOT_SUCCESS) {
        printf("Failed to initialize GPIO for SDA (pin %d)!\n", OLED_GPIO_SDA);
        return -1;
    }
    if (IoTGpioInit(OLED_GPIO_SCL) != IOT_SUCCESS) {
        printf("Failed to initialize GPIO for SCL (pin %d)!\n", OLED_GPIO_SCL);
        return -1;
    }

    // Set GPIO pin functions to I2C
    ret = hi_io_set_func(OLED_GPIO_SDA, OLED_I2C_SDA_FUNC);
    if (ret != IOT_SUCCESS) {
        printf("Failed to set SDA pin function! Error code: %u\n", ret);
        return -1;
    }
    ret = hi_io_set_func(OLED_GPIO_SCL, OLED_I2C_SCL_FUNC);
    if (ret != IOT_SUCCESS) {
        printf("Failed to set SCL pin function! Error code: %u\n", ret);
        return -1;
    }
    
    // Initialize I2C controller
    ret = IoTI2cInit(OLED_I2C_PORT, OLED_I2C_BAUDRATE);
    if (ret != IOT_SUCCESS) {
        printf("I2C master init failed! Error code: %u\n", ret);
        return -1;
    }
    printf("I2C master init success on port %d.\n", OLED_I2C_PORT);

    // Short delay before SSD1306 initialization, as seen in some examples
    usleep(20 * 1000); // 20ms delay

    // Initialize SSD1306 OLED driver
    // Based on the compilation error, ssd1306_Init() is a void function.
    // We cannot check its return value. We assume it handles errors internally
    // or that successful I2C initialization is sufficient for it to proceed.
    ssd1306_Init(); 
    printf("SSD1306 OLED initialization sequence initiated.\n"); // Changed message to reflect no return check

    // It's good practice to clear the screen after init
    ssd1306_Fill(Black);
    ssd1306_UpdateScreen(); // Ensure the clear command is sent

    return 0; // Return success if I2C and GPIO init passed.
}

/**
 * @brief The main task for the OLED display application.
 *
 * This task initializes the OLED, clears it, and then writes the specified strings.
 * @param arg Task argument (not used).
 */
static void OledDisplayTask(void *arg) {
    (void)arg; // Mark argument as unused

    printf("OledDisplayTask started.\n");

    // Initialize OLED and peripherals
    if (OledPeripheralInit() != 0) {
        printf("OLED peripheral initialization failed. Exiting task.\n");
        return;
    }

    // Screen should have been cleared in OledPeripheralInit after ssd1306_Init()
    // ssd1306_Fill(Black); // This might be redundant now but safe
    // ssd1306_UpdateScreen(); 

    // --- Display "你好鸿蒙" ---
    // Set cursor to top-left (x=0, y=0)
    ssd1306_SetCursor(0, 0); 
    // WARNING: Font_7x10 is very unlikely to support Chinese characters.
    // This line will likely display garbled/incorrect output for "你好鸿蒙".
    // A font with Chinese glyphs and appropriate library support is needed for proper display.
    ssd1306_DrawString("你好鸿蒙", Font_7x10, White);
    printf("Attempted to draw '你好鸿蒙'.\n");

    // --- Display "hello world" on a new line ---
    // Font_7x10 is 10 pixels high. Position the next line accordingly.
    // Y=12 gives a small 2-pixel gap. Adjust as needed. Max Y for 128x64 OLED is 63.
    ssd1306_SetCursor(0, 12); 
    ssd1306_DrawString("hello world", Font_7x10, White);
    printf("Attempted to draw 'hello world'.\n");
    
    // Transfer the buffer content to the OLED screen
    ssd1306_UpdateScreen();
    printf("OLED screen updated. Check display.\n");

    // Task can loop here if it needs to do more work or keep running.
    // For a one-shot display, it could also exit.
    // For this example, let it print a message and then loop with delay.
    int count = 0;
    while (1) {
        printf("OLED display task alive... loop %d\n", ++count);
        osDelay(5000); // Delay for 5 seconds (5000 ticks, assuming 1 tick = 1ms)
    }
    // If the task should exit after displaying:
    // printf("OLED display complete. Task will exit.\n");
}

/**
 * @brief Entry point for the OLED Hello World application.
 *
 * This function is called by the OHOS system to start the application feature.
 * It creates a new thread to run the OLED display logic.
 */
static void OledHelloWorldAppEntry(void) {
    osThreadAttr_t threadAttr;

    threadAttr.name = "OledDisplayTask"; // Thread name
    threadAttr.attr_bits = 0U;
    threadAttr.cb_mem = NULL;
    threadAttr.cb_size = 0U;
    threadAttr.stack_mem = NULL;
    threadAttr.stack_size = APP_THREAD_STACK_SIZE; // Defined stack size
    threadAttr.priority = APP_THREAD_PRIORITY;   // Defined priority

    if (osThreadNew((osThreadFunc_t)OledDisplayTask, NULL, &threadAttr) == NULL) {
        printf("[OledHelloWorldAppEntry] FATAL: Failed to create OledDisplayTask!\n");
    } else {
        printf("[OledHelloWorldAppEntry] OledDisplayTask created successfully.\n");
    }
}

// Register the application entry function with the OHOS system.
// The system will call OledHelloWorldAppEntry when it's time to start this feature.
APP_FEATURE_INIT(OledHelloWorldAppEntry);

