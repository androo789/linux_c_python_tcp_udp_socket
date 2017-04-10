//使用pthread_create()创建子线程，在线程处理函数中计算1~100之间的和，保存在sum中，返回该变量的地址，主线程等待子线程结束，并获取退出状态信息，并打印
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
void* Sum(void* pv){
    int i=0,sum=0;
    for(i=1;i<=100;i++)
        sum+=i;
    printf("%d\n",sum);
    return (void*)"success";    //其实return "success"就行
}
int main(){
    pthread_t thread;
    int res=pthread_create(&thread,NULL,Sum,NULL);
    if(0!=res)
        printf("%s",strerror(res)),exit(-1);
    char *retval=NULL;
    res=pthread_join(thread,(void**)&retval);
    if(0!=res)
        printf("%s\n",strerror(res)),exit(-1);
    printf("child thread return : %s\n",retval);
    return 0;
}
