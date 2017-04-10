//02join.c, 使用pthread_join等待目标线程结束
#include<stdio.h>
#include<stdlib.h>
#include<pthread.h>
#include<string.h>      //strerror()//没有这个.h会把strerror(error)当int

void* Calc(void* pv){
    printf("primeter is %lg, area is:%lg\n",2*3.14**(int*)pv,3.14**(int*)pv**(int*)pv);
    return NULL;    //要求有返回值，所以返回NULL
}
main(){
    int* piRadius=(int*)malloc(sizeof(int));

    printf("please input radius\n");
    scanf("%d",piRadius);
    pthread_t thread;
    int error=pthread_create(&thread,NULL,Calc,(void*)piRadius);
    if(0!=error)
        printf("%s\n",strerror(error)),exit(-1);
    error=pthread_join(thread,NULL);
    if(0!=error)
        printf("%s\n",strerror(error)),exit(-1);
    free(piRadius);
    piRadius=NULL;
    return 0;
}
