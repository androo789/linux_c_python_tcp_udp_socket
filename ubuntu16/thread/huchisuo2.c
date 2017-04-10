#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define err_sys(msg) \
    do { perror(msg); exit(-1); } while(0)
#define err_exit(msg) \
    do { fprintf(stderr, msg); exit(-1); } while(0)

int glab = 1;

void *r1(void *arg)
{
    pthread_mutex_t* mutex = (pthread_mutex_t *)arg;
    static int cnt = 10;

    while(cnt--)
    {
        pthread_mutex_lock(mutex);
        glab++;
        printf("I am in r1. cnt = %d\n", glab);
        pthread_mutex_unlock(mutex);
        sleep(1);
    }
    return "r1 over";
}

void *r2(void *arg)
{
    pthread_mutex_t* mutex = (pthread_mutex_t *)arg;
    static int cnt = 10;

    while(cnt--)
    {
        pthread_mutex_lock(mutex);
        glab++;
        printf("I am in r2. cnt = %d\n", glab);
        pthread_mutex_unlock(mutex);
        sleep(1);
    }
    return "r2 over";
}

int main(void)
{
    pthread_mutex_t mutex;
    pthread_t t1, t2;
    char* p1 = NULL;
    char* p2 = NULL;

    if(pthread_mutex_init(&mutex, NULL) < 0)
        err_sys("sem_init error");
    pthread_create(&t1, NULL, r1, &mutex);
    pthread_create(&t2, NULL, r2, &mutex);

    pthread_join(t1, (void **)&p1);
    pthread_join(t2, (void **)&p2);
    pthread_mutex_destroy(&mutex);
    printf("s1: %s\n", p1);
    printf("s2: %s\n", p2);

    return 0;
}
