//使用pthread_create()创建新线程
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>

void* task(void* pv){       //需要一个实参，我们又不需要，就可以给个NULL
    //判断thread中的线程ID是否与pthread_self()的线程ID相等
    int res=pthread_equal(*(pthread_t*)pv,pthread_self());
    if(0!=res)
        printf("these two are Equal\n");
    else
        printf("these two are Unequal\n");
    int i=0;
    for(i=0;i<10;i++){
        printf("I am the new thread\n");
        sleep(1);
    }
}
int main(){
    //1.准备存储线程编号的变量
    pthread_t thread;

    //2.创建新线程，使用pthread_create()
    int errno=pthread_create(&thread,NULL,task,(void*)&thread);//子线程执行完task()就结束了,使用arg把thread传入子线程
    if(0!=errno)
        printf("pthread_create:%s\n",strerror(errno)),exit(-1);
    sleep(10);
    printf("child thread's ID:%lu,parent thread's ID:%lu\n",thread,pthread_self());//pthread_self(),获取当前线程自己的线程编号
    return 0;
}
