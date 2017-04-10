#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define THREAD_NUM 3
#define REPEAT_NUM 3
#define DELAY_TIME_LEVLES 5

pthread_mutex_t mutex;

void *thread_fun(void *arg)
{
    int no = (int) arg;
    int delay;
    int ret,i;

    ret = pthread_mutex_lock(&mutex);  //返回0为成功
    if(ret)
    {
        printf("Thread %d lock failed\n",no);
        pthread_exit(NULL);
    }

    for(i=0;i<REPEAT_NUM;i++)
    {
        delay = (int)(rand()*DELAY_TIME_LEVLES/(RAND_MAX))+1;
        sleep(delay);
        printf("Thread %d: job %d delay = %d\n",no,i,delay);
    }
    printf("Thread %d finished\n",no);
    pthread_exit;
}

int main()
{
    pthread_t thread[THREAD_NUM];
    int i,ret;
    void *thrd;

    srand(time(NULL));

    pthread_mutex_init(&mutex,NULL);

    for(i=0;i<THREAD_NUM;i++)
    {
        ret = pthread_create(&thread[i],NULL,thread_fun,(void *)i);
        if(ret!=0)
        {
            printf("Create %d failed\n",i);
            exit(ret);
        }
    }

    printf("Create thread success!\n Waiting for threads to finish ...\n");

    for(i=0;i<THREAD_NUM;i++)
    {
        ret = pthread_join(thread[i],&thrd);
        pthread_mutex_unlock(&mutex);
    }
    pthread_mutex_destroy(&mutex);
    return 0;
}
