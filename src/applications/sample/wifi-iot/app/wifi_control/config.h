#ifndef CONFIG_H
#define CONFIG_H

// WiFi配置
#define WIFI_SSID       "不要连很卡"    // WiFi名称
#define WIFI_PASSWORD   "00000000" // WiFi密码

// 网络配置
#define SERVER_IP       "192.168.2.32"     // 服务器IP地址
#define SERVER_PORT     8848                // 服务器端口号

// 温湿度数据(全局变量)
extern float g_temperature;  // 当前温度值
extern float g_humidity;     // 当前湿度值

// 超时设置(毫秒)
#define CONNECT_TIMEOUT 5000                // 连接超时时间
#define RESPONSE_TIMEOUT 3000               // 响应超时时间

// 调试模式
#define DEBUG_MODE      1                   // 1=开启调试,0=关闭调试

#endif // CONFIG_H
