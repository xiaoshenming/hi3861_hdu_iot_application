# Hi3861 OpenHarmony 物联网技术及应用

![OS](https://img.shields.io/badge/OS-OpenHarmony-brightgreen?style=for-the-badge) ![platform](https://img.shields.io/badge/platform-Hi3861V100-brightgreen?logo=huawei&style=for-the-badge) ![license](https://img.shields.io/badge/license-Apache-brightgreen?style=for-the-badge) ![star](https://img.shields.io/badge/dynamic/json?style=for-the-badge&color=brightgreen&logo=Gitee&label=stars&url=https://gitee.com/api/v5/repos/HiSpark/hi3861_hdu_iot_application&query=$.stargazers_count) ![watch](https://img.shields.io/badge/dynamic/json?style=for-the-badge&color=brightgreen&logo=Gitee&label=watch&url=https://gitee.com/api/v5/repos/HiSpark/hi3861_hdu_iot_application&query=$.watchers_count) ![fork](https://img.shields.io/badge/dynamic/json?style=for-the-badge&color=brightgreen&logo=Gitee&label=fork&url=https://gitee.com/api/v5/repos/HiSpark/hi3861_hdu_iot_application&query=$.forks_count)



# 介绍
欢迎使用Hi3861V100开发OpenHarmony物联网应用.[](https://developer.huawei.com/consumer/cn/training/course/mooc/C101641968823265204?refresh=166942862398)

#  hispark_T1介绍

## 一、硬件说明
这是一个嵌入式软件项目, 所以你需要有一块Hi3861V100的开发板. 如果你只是想学习使用Hi3861开发WiFi物联网应用, 那么基本上任何一块基于Hi3861V100的开发板都是可以的. 如果你需要使用这个项目中的全部外设实验用例, 例如陀螺仪, NFC, 马达控制, 平衡车等等, 那么你需要配合一块HiSpark T1开发板使用.

<div align=center><img src="doc/pic/iot_car_t1.jpg" alt="iot_car" height="200" />    <img src="doc/pic/balance_car.jpg" alt="iot_car" height="200"/>   </div>

## 二、快速上手（十分钟上手）

以下教程将教您快速的体验OpenHarmony Hi3861V100的源码`[获取]`、`[编译]`、`[烧录]`,`[hispark_T1案例使用]`。


* [《物联网技术及应用实验指导手册》第1.1章节Windows环境搭建](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/doc/%E7%89%A9%E8%81%94%E7%BD%91%E6%8A%80%E6%9C%AF%E5%8F%8A%E5%BA%94%E7%94%A8%E5%AE%9E%E9%AA%8C%E6%8C%87%E5%AF%BC%E6%89%8B%E5%86%8C.pdf)
* [hispark_T1硬件原理图](https://gitee.com/HiSpark/hi3861_hdu_iot_application/tree/master/doc/board)

## 三、hispark_T1 案例开发

这是一个嵌入式软件项目, 所以你需要有一块Hi3861V100的开发板. 如果你只是想学习使用Hi3861开发WiFi物联网应用, 那么基本上任何一块基于Hi3861V100的开发板都是可以的. 如果你需要使用这个项目中的全部外设实验用例, 例如陀螺仪, NFC, 马达控制, 平衡车等等, 那么你需要配合一块HiSpark T1开发板使用.

* 《物联网技术及应用实验指导手册》：[hispark_T1案例使用](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/doc/%E7%89%A9%E8%81%94%E7%BD%91%E6%8A%80%E6%9C%AF%E5%8F%8A%E5%BA%94%E7%94%A8%E5%AE%9E%E9%AA%8C%E6%8C%87%E5%AF%BC%E6%89%8B%E5%86%8C.pdf)

例程列表如下所示：

| 例程名              | 说明                               | 路径                                       |
| ------------------- | ---------------------------------- | ------------------------------------------ |
| led_demo            | 红色LED闪亮                        | 《物联网技术及应用实验指导手册》第3.1章节  |
| encoder_demo        | 编码器                             | 《物联网技术及应用实验指导手册》第3.2章节  |
| tricolor_lamp_demo  | 小车大灯实现红、绿、蓝、白循环闪亮 | 《物联网技术及应用实验指导手册》第3.3章节  |
| button_demo         | IO扩展芯片实现按键功能             | 《物联网技术及应用实验指导手册》第3.4章节  |
| hcsr04_demo         | 超声波                             | 《物联网技术及应用实验指导手册》第3.5章节  |
| motor_demo          | PWM马达转动                        | 《物联网技术及应用实验指导手册》第3.6章节  |
| sg92r_demo          | 舵机90°、0°、-90°                  | 《物联网技术及应用实验指导手册》第3.7章节  |
| cw2015_demo         | 电源管理芯片                       | 《物联网技术及应用实验指导手册》第3.8章节  |
| rtc_demo            | 万年历                             | 《物联网技术及应用实验指导手册》第3.8章节  |
| nfc_demo            | 手机与NFC通信                      | 《物联网技术及应用实验指导手册》第3.9章节  |
| i2c_bus_demo        | I2C总线实验                        | 《物联网技术及应用实验指导手册》第3.10章节 |
| lth1550_demo        | ADC实验模拟信号转为数字信号        | 《物联网技术及应用实验指导手册》第3.11章节 |
| uart_demo           | 串口通信                           | 《物联网技术及应用实验指导手册》第3.12章节 |
| wifi_demo           | WiFi热点创建和WiFi联网             | 《物联网技术及应用实验指导手册》第3.13章节 |
| thread_demo         | 线程                               | 《物联网技术及应用实验指导手册》第4.1章节  |
| semaphore_demo      | 信号量                             | 《物联网技术及应用实验指导手册》第4.2章节  |
| timer_demo          | 定时器                             | 《物联网技术及应用实验指导手册》第4.3章节  |
| interrupt_demo      | 中断实验                           | 《物联网技术及应用实验指导手册》第4.4章节  |
| ultrasonic_car_demo | 超声波避障小车                     | 《物联网技术及应用实验指导手册》第5.1章节  |
| trace_demo          | 循迹小车                           | 《物联网技术及应用实验指导手册》第5.2章节  |
| trace_ex_demo       | IO扩展芯片实现循迹小车             | 《物联网技术及应用实验指导手册》第5.2章节  |
| imu_square_demo     | 基于陀螺仪YAW角控制小车走正方形    | 《物联网技术及应用实验指导手册》第5.3章节  |
| balance_car_demo    | 平衡车                             | 《物联网技术及应用实验指导手册》第5.4章节  |
| histreaming_demo    | 手机控制小车                       | 《物联网技术及应用实验指导手册》第5.5章节  |
| oc_demo             | 基于华为IoT云平台的智能小车实验    | 《物联网技术及应用实验指导手册》第5.6章节  |
|                     |                                    |                                            |

​	

#  hispark_pegasus介绍

## 一、硬件说明

![image-20230201170309577](doc/pic/image-20230201170309577.png)

## 二、快速上手（十分钟上手）

以下教程将教您快速的体验OpenHarmony Hi3861V100的源码`[获取]`、`[编译]`、`[烧录]`。


* [hispark_pegasus十分钟上手](quick-start/BearPi-HM_Nano十分钟上手.md)
* [hisaprk_pegasus硬件原理图]()

## 三、hispark_pegasus 案例开发

BearPi-HM Nano提供多个案例，案例以A、B、C、D进行不同类别进行分级，方便初学者由浅入深逐步学习。您拿到工程后经过简单的编程和下载即可看到实验现象。下面依次对A/B/C/D类进行介绍：

例程列表如下所示：

| 例程名           | 说明                                                         |
| ---------------- | ------------------------------------------------------------ |
| 00_thread        | [线程](https://gitee.com/HiSpark/hi3861_hdu_iot_application/tree/master/src/vendor/hihope/hispark_pegasus/demo/00_thread) |
| 01_timer         | [定时器  ](https://gitee.com/HiSpark/hi3861_hdu_iot_application/tree/master/src/vendor/hihope/hispark_pegasus/demo/01_timer) |
| 02_delay         | [延时](https://gitee.com/HiSpark/hi3861_hdu_iot_application/tree/master/src/vendor/hihope/hispark_pegasus/demo/02_delay) |
| 03_mutex         | [互斥锁](https://gitee.com/HiSpark/hi3861_hdu_iot_application/tree/master/src/vendor/hihope/hispark_pegasus/demo/03_mutex) |
| 04_semaphore     | [信号量](https://gitee.com/HiSpark/hi3861_hdu_iot_application/tree/master/src/vendor/hihope/hispark_pegasus/demo/04_semaphore) |
| 05_message       | [消息队列](https://gitee.com/HiSpark/hi3861_hdu_iot_application/tree/master/src/vendor/hihope/hispark_pegasus/demo/05_message) |
| 06_gpioled       | [红色 LED 不间断烁](https://gitee.com/HiSpark/hi3861_hdu_iot_application/tree/master/src/vendor/hihope/hispark_pegasus/demo/06_gpioled) |
| 07_gpiobutton    | [按键控制LED灯亮灭](https://gitee.com/HiSpark/hi3861_hdu_iot_application/tree/master/src/vendor/hihope/hispark_pegasus/demo/07_gpiobutton) |
| 08_pwmled        | [炫彩灯板的三色灯会呈现红色，并且亮度会逐渐变化：暗--亮](https://gitee.com/HiSpark/hi3861_hdu_iot_application/tree/master/src/vendor/hihope/hispark_pegasus/demo/08_pwmled) |
| 09_adc           | [有光时，串口输出的ADC的值为120左右；无光时，串口输出的ADC的值为1800左右](https://gitee.com/HiSpark/hi3861_hdu_iot_application/tree/master/src/vendor/hihope/hispark_pegasus/demo/09_adc) |
| 10_i2caht20      | [温湿度监测](https://gitee.com/HiSpark/hi3861_hdu_iot_application/tree/master/src/vendor/hihope/hispark_pegasus/demo/10_i2caht20) |
| 11_uart          | [UART自发自收](https://gitee.com/HiSpark/hi3861_hdu_iot_application/tree/master/src/vendor/hihope/hispark_pegasus/demo/11_uart) |
| 14_pwmbeer       | [蜂鸣器鸣响](https://gitee.com/HiSpark/hi3861_hdu_iot_application/tree/master/src/vendor/hihope/hispark_pegasus/demo/14_pwmbeer) |
| 15_pwmbeermusic  | [蜂鸣器会响](https://gitee.com/HiSpark/hi3861_hdu_iot_application/tree/master/src/vendor/hihope/hispark_pegasus/demo/15_pwmbeermusic) |
| 16_trafficlight  | [交通灯板红黄绿三个灯循环闪烁四次](https://gitee.com/HiSpark/hi3861_hdu_iot_application/tree/master/src/vendor/hihope/hispark_pegasus/demo/16_trafficlight) |
| 17_colorfullight | [炫彩灯板的三色灯会先依次按照红绿蓝三色闪烁两次](https://gitee.com/HiSpark/hi3861_hdu_iot_application/tree/master/src/vendor/hihope/hispark_pegasus/demo/17_colorfullight) |
| 21_tcpclient     | [tcp客户端](https://gitee.com/HiSpark/hi3861_hdu_iot_application/tree/master/src/vendor/hihope/hispark_pegasus/demo/21_tcpclient) |
| 22_tcpserver     | [tcp服务端](https://gitee.com/HiSpark/hi3861_hdu_iot_application/tree/master/src/vendor/hihope/hispark_pegasus/demo/22_tcpserver) |
| 23_udpclient     | [udp客户端](https://gitee.com/HiSpark/hi3861_hdu_iot_application/tree/master/src/vendor/hihope/hispark_pegasus/demo/23_udpclient) |
| 24_udpserver     | [udp服务端](https://gitee.com/HiSpark/hi3861_hdu_iot_application/tree/master/src/vendor/hihope/hispark_pegasus/demo/24_udpserver) |
| 28_easy_wifi     | [WiFi热点创建和WiFi联网](https://gitee.com/HiSpark/hi3861_hdu_iot_application/tree/master/src/vendor/hihope/hispark_pegasus/demo/28_easy_wifi) |
|                  |                                                              |



# BearPi-HM Nano介绍

## 一、硬件说明

小熊派·季[（BearPi-HM Nano）](https://item.taobao.com/item.htm?id=633296694816)是一款由小熊派专为OpenHarmony系统打造的开发板，如下图所示

* 采用IoTCluB的E53接口标准，可接入所有的[E53案例扩展板](https://shop336827451.taobao.com/category-1488778768.htm)。
* 主控为WiFi SoC Hi3861，160MHz主频，SRAM 352KB、ROM 288KB、2M Flash。
* 板载NFC Forum Type 2 Tag芯片及天线，可实现OpenHarmony“碰一碰”功能。
* 一根TypeC USB线，即可实现供电、下载、调试等多种功能。

[![](src/vendor/bearpi/bearpi_hm_nano/doc/figures/00_public/BearPi-HM_Nano_Info.png)

## 二、快速上手（十分钟上手）

以下教程将教您快速的体验OpenHarmony Hi3861V100的源码`[获取]`、`[编译]`、`[烧录]`。


* [BearPi-HM_Nano十分钟上手](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/doc/BearPi-HM_Nano%E5%8D%81%E5%88%86%E9%92%9F%E4%B8%8A%E6%89%8B.md)
* [BearPi-HM_Nano硬件原理图](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/doc/BearPi-HM_Nano%E5%8D%81%E5%88%86%E9%92%9F%E4%B8%8A%E6%89%8B.md)

## 三、BearPi-HM_Nano 案例开发

BearPi-HM Nano提供多个案例，案例以A、B、C、D进行不同类别进行分级，方便初学者由浅入深逐步学习。您拿到工程后经过简单的编程和下载即可看到实验现象。下面依次对A/B/C/D类进行介绍：

* `A1 - A99`：内核类
* `B1 - B99`：基础外设类
* `C1 - C99`：E53传感器类
* `D1 - D99`：物联网类

例程列表如下所示：

| 编号 | 类别      | 例程名                     | 说明                                                         |
| ---- | --------- | -------------------------- | ------------------------------------------------------------ |
| A1   | 内核      | thread                     | [任务交替打印](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/A1_kernal_thread) |
| A2   | 内核      | timer                      | [定时器  ](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/A2_kernel_timer) |
| A3   | 内核      | event                      | [事件](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/A3_kernel_event) |
| A4   | 内核      | mutex                      | [互斥锁](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/A4_kernel_mutex) |
| A5   | 内核      | semp                       | [信号量](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/A5_kernel_semaphore) |
| A6   | 内核      | message                    | [消息队列](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/A6_kernel_message) |
| B1   | 基础      | led_blink                  | [红色 LED 不间断烁](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/B1_basic_led_blink) |
| B2   | 基础      | button                     | [按键控制LED灯亮灭](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/B2_basic_button) |
| B3   | 基础      | pwm_led                    | [红色 LED 呼吸灯](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/B3_basic_pwm_led) |
| B4   | 基础      | adc_mq2                    | [ADC读取电压](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/B4_basic_adc) |
| B5   | 基础      | i2c_bh1750                 | [I2C读取NFC标签](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/B5_basic_i2c_nfc) |
| B6   | 基础      | basic_uart                 | [UART自发自收](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/B6_basic_uart) |
| C1   | E53传感器 | e53_sf1_example            | [驱动烟雾传感器 MQ2](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/C1_e53_sf1_mq2) |
| C2   | E53传感器 | e53_ia1_example            | [驱动温湿度传感器 SHT30](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/C2_e53_ia1_temp_humi_pls) |
| C3   | E53传感器 | e53_sc1_example            | [驱动光强传感器 BH1750](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/C3_e53_sc1_pls) |
| C4   | E53传感器 | e53_sc2_example            | [驱动 6 轴陀螺仪 MPU6050](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/C4_e53_sc2_axis) |
| C5   | E53传感器 | e53_is1_example            | [驱动人体红外传感器](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/C5_e53_is1_infrared) |
| D1   | 物联网    | iot_wifi_ap                | [Wifi热点创建](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/D1_iot_wifi_ap) |
| D2   | 物联网    | iot_wifi_sta_connect       | [Wifi联网](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/D2_iot_wifi_sta_connect) |
| D3   | 物联网    | udp_client                 | [使用 Socket 实现 UDP 客户端](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/D3_iot_udp_client) |
| D4   | 物联网    | tcp_server                 | [使用 Socket 实现 TCP 服务端](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/D4_iot_tcp_server) |
| D5   | 物联网    | iot_mqtt                   | [使用 Paho-MQTT 软件包实现 MQTT 协议通信](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/D5_iot_mqtt) |
| D6   | 物联网    | iot_cloud_oc_sample        | [接入华为IoT 云平台](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/D6_iot_cloud_oc) |
| D7   | 物联网    | iot_cloud_oc_smoke         | [基于华为IoT平台的智慧烟感案例](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/D7_iot_cloud_oc_smoke) |
| D8   | 物联网    | iot_cloud_oc_light         | [基于华为IoT平台的智慧路灯案例](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/D8_iot_cloud_oc_light) |
| D9   | 物联网    | iot_cloud_oc_manhole_cover | [基于华为IoT平台的智慧井盖案例](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/D9_iot_cloud_oc_manhole_cover) |
| D10  | 物联网    | iot_cloud_oc_infrared      | [基于华为IoT平台的智慧人体感应案例](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/D10_iot_cloud_oc_infrared) |
| D11  | 物联网    | iot_cloud_oc_agriculture   | [基于华为IoT平台的智慧农业案例](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/D11_iot_cloud_oc_agriculture) |
| D12  | 物联网    | iot_cloud_oc_gps           | [基于华为IoT平台的智慧物流案例](https://gitee.com/HiSpark/hi3861_hdu_iot_application/blob/master/src/vendor/bearpi/bearpi_hm_nano/demo/D12_iot_cloud_oc_gps) |
|      |           |                            |                                                              |

# 问题与解答

如果你对项目中的代码或者文档存在疑问, 欢迎在Issues中提出你的问题(别忘了先在FAQ中看一看是否已经有答案了😎). 如果你自己解决了一个了不起的问题, 非常欢迎你把问题和解决方法发到Issues里, 如果你看到别人的问题而你正好有答案, 也欢迎你帮助解答其他人的问题, 所谓"授人玫瑰手有余香"嘛.

# 参与贡献
我们非常欢迎你能对这个项目提出代码上的改进或扩展, 方法是:
1.  Fork 本仓库
2.  下载到本地, 修改, 提交
3.  推送代码
4.  在页面点击 Pull Request

这样我们就能接到你的推送申请.

# 最后的话
OpenHarmony还是一个相当年轻的系统, 还在快速的发展中, 所以在这个过程中, 不可避免的你会遇到不少问题, 有些可能还是相当让人下头的那种:confounded:, 不过不要担心, 你可以多尝试几种方法去研究去解决, 也可以用搜索引擎搜索答案, 你当然也可以回到这里提出你的问题, 我们和其他小伙伴会尽力帮助你. 但最重要的是要记得: 所有那些让人仰望的技术大神, 其实都是从这样的阶段磨砺成长起来的. :rainbow:

最后的最后, 欢迎来到OpenHarmony的世界探险!
