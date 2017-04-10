//03join.c  使用pthread_join获取目标线程的退出状态
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
void* task(void* pv){
    char* pc="hello";
    return (void*)pc;
}
main(){
    //创建子线程，使用pthread_create
    pthread_t thread;
    int error=pthread_create(&thread,NULL,task,NULL);
    if(0!=error)
        printf("%s",strerror(error)),exit(-1);
    //等待子线程结束，并获取退出状态信息
    char* ps=NULL;
    error=pthread_join(thread,(void**)&ps);
    if(0!=error)
        printf("pthread_join %s\n",strerror(error)),exit(-1);
    printf("child thread returned:%s\n",ps);
    return 0;
}
