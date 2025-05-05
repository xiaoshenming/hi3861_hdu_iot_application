#include<stdio.h>
#include<ohos_init.h>
#include<cmsis_os2.h>
#include<unistd.h>
void mytask_func(void *arg) 
{
    int n = 0;
    while(1) {
        printf("Hello, world! %d\n", n++);
        usleep(5000000);}
}  
void mytask_func1(void *arg) 
{
    int n = 0;
    while(1) {
        printf("万岁芜湖! %d\n", n++);
        usleep(5000000);}
}  
void myhello_func()
{
    osThreadAttr_t attr;
    memset(&attr, 0, sizeof(attr));
    attr.name = "mytask";
    attr.stack_size = 4096;
    attr.priority = 11;
    osThreadNew(mytask_func, NULL, &attr);
    osThreadNew(mytask_func1, NULL, &attr);
    // printf("Hello, world!\n");
}
SYS_RUN(myhello_func);
// APP_FEATURE_INIT(myhello_func);
//标注了APP_FEATURE_INIT宏，该宏会将myhello_func函数注册到系统初始化过程中，并在系统初始化完成后调用该函数。