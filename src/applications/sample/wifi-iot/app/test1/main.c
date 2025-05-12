#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "hi_io.h"
#include "hi_i2c.h"
#include "iot_gpio.h"
#include "iot_i2c.h"

// --- Configuration ---
#define OLED_I2C_IDX HI_I2C_IDX_0 // I2C Bus ID (usually 0 for Hi3861)
#define OLED_I2C_ADDR 0x3C       // Standard I2C Address for many 128x64 OLEDs (0x78 >> 1)
#define OLED_I2C_BAUDRATE 400000 // 400kHz

// GPIO pins for I2C0 on Hi3861
#define I2C0_SDA_PIN HI_IO_NAME_GPIO_13
#define I2C0_SCL_PIN HI_IO_NAME_GPIO_14
#define I2C0_SDA_FUNC HI_IO_FUNC_GPIO_13_I2C0_SDA
#define I2C0_SCL_FUNC HI_IO_FUNC_GPIO_14_I2C0_SCL

// OLED Controller Commands (SSD1306)
#define OLED_CMD 0x00  // Command Mode
#define OLED_DATA 0x40 // Data Mode

// Basic SSD1306 Initialization Sequence
static const uint8_t oled_init_cmds[] = {
    0xAE, // Display OFF
    0xD5, // Set Display Clock Divide Ratio/Oscillator Frequency
    0x80, // Suggested ratio (0x80)
    0xA8, // Set MUX Ratio
    0x3F, // 1/64 duty
    0xD3, // Set display offset
    0x00, // No offset
    0x40, // Set start line address (0x40 = Line 0)
    0x8D, // Charge Pump Setting
    0x14, // Enable charge pump
    0x20, // Set Memory Addressing Mode
    0x00, // Horizontal addressing mode
    0xA1, // Set Segment Re-map (A0=Normal, A1=Re-mapped) -> Column address 127 is mapped to SEG0
    0xC8, // Set COM Output Scan Direction (C0=Normal, C8=Re-mapped) -> Scan from COM[N-1] to COM0
    0xDA, // Set COM Pins Hardware Configuration
    0x12, // Alternative COM pin configuration, disable COM Left/Right remap
    0x81, // Set Contrast Control
    0xCF, // Default contrast
    0xD9, // Set Pre-charge Period
    0xF1, // Pre-charge: 15 clocks, Discharge: 1 clock
    0xDB, // Set VCOMH Deselect Level
    0x40, // VCOMH = ~0.77 * Vcc
    0xA4, // Entire Display ON (A4=Output follows RAM, A5=Output ignores RAM)
    0xA6, // Set Normal Display (A6=Normal, A7=Inverse)
    0xAF  // Display ON
};

// Simple 5x7 ASCII Font (Subset for "Hello World!")
static const uint8_t font5x7[][5] = {
    {0x00, 0x00, 0x00, 0x00, 0x00}, // Space 0x20
    {0x00, 0x00, 0x5F, 0x00, 0x00}, // ! 0x21
    // ... Add other characters if needed
    {0x7F, 0x08, 0x08, 0x08, 0x7F}, // H 0x48
    {0x00, 0x41, 0x7F, 0x41, 0x00}, // W 0x57
    // ...
    {0x3E, 0x41, 0x41, 0x41, 0x22}, // d 0x64
    {0x7C, 0x08, 0x04, 0x04, 0x78}, // e 0x65
    {0x00, 0x41, 0x7F, 0x40, 0x00}, // l 0x6C
    {0x3E, 0x41, 0x41, 0x22, 0x1C}, // o 0x6F
    {0x7C, 0x08, 0x04, 0x04, 0x08}  // r 0x72
};

// --- I2C Helper Functions ---

/**
 * @brief Sends data via I2C.
 * @param data Pointer to the data buffer.
 * @param len Length of the data to send.
 * @return 0 on success, non-zero on failure.
 */
static uint32_t OledI2cWrite(const uint8_t *data, uint32_t len)
{
    return IoTI2cWrite(OLED_I2C_IDX, (OLED_I2C_ADDR << 1) | 0, data, len);
}

/**
 * @brief Sends a command to the OLED.
 * @param cmd The command byte.
 */
static void OledWriteCmd(uint8_t cmd)
{
    uint8_t buffer[2] = {OLED_CMD, cmd};
    uint32_t retval = OledI2cWrite(buffer, sizeof(buffer));
    if (retval != 0) {
        printf("ERROR: I2C write cmd failed! Errno: %u\n", retval);
    }
}

/**
 * @brief Sends data to the OLED RAM.
 * @param data The data byte.
 */
static void OledWriteData(uint8_t data)
{
    uint8_t buffer[2] = {OLED_DATA, data};
    uint32_t retval = OledI2cWrite(buffer, sizeof(buffer));
    if (retval != 0) {
        printf("ERROR: I2C write data failed! Errno: %u\n", retval);
    }
}

// --- OLED Control Functions ---

/**
 * @brief Initializes the OLED display.
 */
static void OledInit(void)
{
    // Initialize GPIO pins for I2C
    IoTGpioInit(I2C0_SDA_PIN);
    IoTGpioInit(I2C0_SCL_PIN);
    hi_io_set_func(I2C0_SDA_PIN, I2C0_SDA_FUNC);
    hi_io_set_func(I2C0_SCL_PIN, I2C0_SCL_FUNC);

    // Initialize I2C peripheral
    uint32_t retval = IoTI2cInit(OLED_I2C_IDX, OLED_I2C_BAUDRATE);
    if (retval != 0) {
        printf("ERROR: I2C Init failed! Errno: %u\n", retval);
        return;
    }
    printf("I2C Init Success\n");

    // Short delay before sending commands
    usleep(20000); // 20ms

    // Send initialization commands
    for (uint32_t i = 0; i < sizeof(oled_init_cmds); i++) {
        OledWriteCmd(oled_init_cmds[i]);
    }
    printf("OLED Init Commands Sent\n");
}

/**
 * @brief Clears the OLED screen RAM.
 */
static void OledClear(void)
{
    for (uint8_t page = 0; page < 8; page++) { // 8 pages (rows)
        OledWriteCmd(0xB0 + page); // Set page address
        OledWriteCmd(0x00);       // Set lower column address
        OledWriteCmd(0x10);       // Set higher column address
        for (uint8_t col = 0; col < 128; col++) { // 128 columns
            OledWriteData(0x00); // Write 0 to clear
        }
    }
    printf("OLED Cleared\n");
}

/**
 * @brief Sets the cursor position on the OLED.
 * @param page Page number (0-7).
 * @param col Column number (0-127).
 */
static void OledSetPosition(uint8_t page, uint8_t col)
{
    if (page > 7) page = 7;
    if (col > 127) col = 127;
    OledWriteCmd(0xB0 + page);         // Set page address
    OledWriteCmd(0x00 | (col & 0x0F)); // Set lower column address
    OledWriteCmd(0x10 | (col >> 4));   // Set higher column address
}

/**
 * @brief Displays a character at the current position.
 * @param c Character to display.
 */
static void OledShowChar(char c)
{
    // Basic ASCII check and offset
    if (c < ' ' || c > '~') {
        c = ' '; // Default to space if out of range
    }
    uint8_t char_index = c - ' '; // Index into font table

    // Check if character exists in our limited font table
    // (You might need a more robust check or a complete font table)
    const uint8_t *font_data;
    switch(c) {
        case 'H': font_data = font5x7['H' - ' ']; break;
        case 'e': font_data = font5x7['e' - ' ']; break;
        case 'l': font_data = font5x7['l' - ' ']; break;
        case 'o': font_data = font5x7['o' - ' ']; break;
        case ' ': font_data = font5x7[0]; break; // Space
        case 'W': font_data = font5x7['W' - ' ']; break;
        case 'r': font_data = font5x7['r' - ' ']; break;
        case 'd': font_data = font5x7['d' - ' ']; break;
        case '!': font_data = font5x7['!' - ' ']; break;
        default:  font_data = font5x7[0]; break; // Default to space
    }


    for (uint8_t i = 0; i < 5; i++) { // Font width = 5
        OledWriteData(font_data[i]);
    }
    OledWriteData(0x00); // Add a 1-pixel gap between characters
}

/**
 * @brief Displays a string at the specified position.
 * @param page Page number (0-7).
 * @param col Starting column number (0-127).
 * @param str The null-terminated string to display.
 */
static void OledShowString(uint8_t page, uint8_t col, const char *str)
{
    OledSetPosition(page, col);
    while (*str) {
        OledShowChar(*str++);
        // Basic boundary check (doesn't handle wrapping)
        col += 6; // 5 font width + 1 gap
        if (col > 127) break; // Stop if exceeding screen width
    }
}

// --- Task Function ---

/**
 * @brief Main task to initialize and display on OLED.
 */
void OledTask(void *arg)
{
    (void)arg; // Unused parameter

    printf("OLED Task Started\n");

    // Initialize OLED
    OledInit();

    // Clear the screen
    OledClear();
    usleep(10000); // Small delay

    // Display "Hello World!"
    // Parameters: page (0-7), column (0-127), string
    OledShowString(3, 20, "Hello World!"); // Display roughly centered vertically and horizontally
    printf("Displayed 'Hello World!' on OLED\n");

    // Task finished its main job, can loop or exit
    while (1) {
        osDelay(1000); // Keep task alive if needed, or remove loop
    }
}

// --- Application Entry ---

/**
 * @brief Main application entry point.
 */
static void OledHelloWorldEntry(void)
{
    osThreadAttr_t attr;

    printf("Starting OLED Hello World Example\n");

    memset(&attr, 0, sizeof(attr));
    attr.name = "OledTask";
    attr.stack_size = 4096; // Allocate sufficient stack
    attr.priority = osPriorityNormal; // Set appropriate priority

    if (osThreadNew(OledTask, NULL, &attr) == NULL) {
        printf("ERROR: Failed to create OledTask!\n");
    } else {
        printf("OledTask created successfully\n");
    }
}

// Register the application entry point
SYS_RUN(OledHelloWorldEntry);
