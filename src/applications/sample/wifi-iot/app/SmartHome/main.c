#include<stdio.h>
#include<ohos_init.h>
#include<cmsis_os2.h>
#include<unistd.h>
void myhello_func()
{
    osThreadAttr_t attr;
    printf("Hello, world!\n");
}
SYS_RUN(myhello_func);
