#include <stdio.h>
#include <ohos_init.h>
#include <unistd.h>
#include <hi_io.h>
#include <hi_gpio.h>
#include <iot_gpio.h>
#include <hi_time.h>
#include <iot_pwm.h>
#include <hi_pwm.h>

#define BUZZER_PIN HI_GPIO_IDX_9  // 蜂鸣器连接的GPIO引脚
#define PWM_CLOCK_SRC 160000000    // 外部晶振48MHz

// 音调频率值
const int tones[] = {
    0,     // 占位
    38223, // 音符1
    34052, // 音符2
    30338, // 音符3
    28635, // 音符4
    25511, // 音符5
    22728, // 音符6
    20249, // 音符7
    51021  // 音符8
};

void myhello_func()
{
    printf("Buzzer Demo Start\n");
    
    // GPIO初始化
    IoTGpioInit(BUZZER_PIN);
    // 设置GPIO功能为PWM输出
    hi_io_set_func(BUZZER_PIN, HI_IO_FUNC_GPIO_9_PWM0_OUT);
    IoTGpioSetDir(BUZZER_PIN, IOT_GPIO_DIR_OUT);

    // PWM初始化
    IoTPwmInit(0);               // 使用PWM通道0
    hi_pwm_set_clock(PWM_CLK_XTAL); // 使用外部晶振时钟源
    
    // 循环播放音符1-8
    for(int i=1; i<=8; i++) {
        printf("Playing note %d\n", i);
        
        // 计算PWM频率 (时钟源频率/音调频率)
        int freq = PWM_CLOCK_SRC / tones[i];
        // 启动PWM（通道0，占空比80%）
        IoTPwmStart(0, 30, freq);
        // 持续1秒
        hi_udelay(1000000); 
        // 停止PWM
        IoTPwmStop(0);
    }
}

SYS_RUN(myhello_func);
