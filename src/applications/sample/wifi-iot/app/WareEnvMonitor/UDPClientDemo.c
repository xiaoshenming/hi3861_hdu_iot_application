#include <stdio.h>
#include <unistd.h>

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "wifi_device.h"
#include "lwip/netifapi.h"
#include "lwip/api_shell.h"
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include "lwip/sockets.h"//套接字的头文件
#include "wifi_connecter.h"

#include "config.h"

// #define _PROT_ 8888//端口对应上面的“8888”

//在sock_fd 进行监听，在 new_fd 接收新的链接
int sock_fd;

int addr_length;
static char send_data[128];

static void UDPClientTask(void)
{
    //初始化服务器的地址信息的结构体
    struct sockaddr_in send_addr;
    socklen_t addr_length = sizeof(send_addr);
    char recvBuf[512];

    //连接Wifi
    printf("Connecting to WiFi %s...\n", WIFI_SSID);
    WifiDeviceConfig apConfig = {0};
    strcpy(apConfig.ssid, WIFI_SSID);
    strcpy(apConfig.preSharedKey, WIFI_PASSWORD);
    apConfig.securityType = WIFI_SEC_TYPE_PSK;
    
    int netId = ConnectToHotspot(&apConfig);
    if (netId < 0) {
        printf("Failed to connect WiFi!\n");
        return;
    }
    printf("WiFi connected successfully!\n");
    //创建本机的套接字socket（一对中的其一）
    if ((sock_fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        perror("create socket failed!\r\n");
        exit(1);
    }

    printf("Initializing server address: %s:%d\n", SERVER_IP, SERVER_PORT);
    printf("Current temperature: %.2fC, humidity: %.2f%%\n", g_temperature, g_humidity);
    //初始化预连接的服务端地址，绑定服务端的IP以及应用进程端口
    //IPV4
    send_addr.sin_family = AF_INET;
    //通信另一端的应用进程端口
    send_addr.sin_port = htons(SERVER_PORT);
    //通信另一端的IP地址
    send_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    addr_length = sizeof(send_addr);

    //总计发送 count 次数据
    while (1)
    {
        bzero(recvBuf, sizeof(recvBuf));

        // 构造温湿度数据并发送
        snprintf(send_data, sizeof(send_data), "Temperature: %.2fC, Humidity: %.2f%%\r\n", g_temperature, g_humidity);
        printf("Sending data to server: %s\n", send_data);
        int sent = sendto(sock_fd, send_data, strlen(send_data), 0, (struct sockaddr *)&send_addr, addr_length);
        printf("Sent %d bytes\n", sent);

        //线程休眠一段时间
        sleep(1);

        // //接收服务端返回的字符串
        // recvfrom(sock_fd, recvBuf, sizeof(recvBuf), 0, (struct sockaddr *)&send_addr, &addr_length);
        // printf("**********************");
        // printf("%s:%d=>%s\n", inet_ntoa(send_addr.sin_addr), ntohs(send_addr.sin_port), recvBuf);
    }

    //关闭这个 socket
    // closesocket(sock_fd);
}

static void UDPClientDemo(void)
{
    osThreadAttr_t attr;
    printf("my UDP test\n");
    osDelay(100);

    attr.name = "UDPClientTask";
    attr.attr_bits = 0U;
    attr.cb_mem = NULL;
    attr.cb_size = 0U;
    attr.stack_mem = NULL;
    attr.stack_size = 10240;
    attr.priority = osPriorityNormal;

    if (osThreadNew((osThreadFunc_t)UDPClientTask, NULL, &attr) == NULL)
    {
        printf("[UDPClientDemo] Falied to create UDPClientTask!\n");
    }
}

APP_FEATURE_INIT(UDPClientDemo);
