
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define ERR_EXIT(m) \
    do { \
        perror(m); \
        exit(EXIT_FAILURE); \
    } while (0)

//int a = 200;
//int b = 100;
//pthread_mutex_t lock;

void* ThreadA(void* arg)//server
{
//pthread_mutex_lock(&lock);          //锁
//a -= 50;
//sleep(5);                                      //执行到一半 使用sleep 放弃cpu调度
//b += 50;
//pthread_mutex_unlock(&lock);

    int sock_ser;
    if ((sock_ser = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        ERR_EXIT("socket error");

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5188);
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sock_ser, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        ERR_EXIT("bind error");

    char recvbuf[1024] = {0};
    struct sockaddr_in peeraddr;
    socklen_t peerlen;
    int n;

    while (1)
    {

        peerlen = sizeof(peeraddr);
        memset(recvbuf, 0, sizeof(recvbuf));
        printf("server befer recvfrom\n");
        n = recvfrom(sock_ser, recvbuf, sizeof(recvbuf), 0,
                     (struct sockaddr *)&peeraddr, &peerlen);
	    printf("server after recvfrom\n");
        if (n == -1)
        {

            if (errno == EINTR)
                continue;

            ERR_EXIT("recvfrom error");
        }
        else if(n > 0)
        {
            printf("server printf ------------\n");
            fputs(recvbuf, stdout);
            sendto(sock_ser, recvbuf, n, 0,
                   (struct sockaddr *)&peeraddr, peerlen);
        }
    }
    close(sock_ser);
}

void* ThreadB(void* arg)// client
{
//sleep(1);                            //放弃CPU调度 目的先让A线程运行。
//pthread_mutex_lock(&lock);
//printf("%d\n", a + b);
//pthread_mutex_unlock(&lock);

    int sock_cli;
    if ((sock_cli = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        ERR_EXIT("socket");

        struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(5188);
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    int ret;
    char sendbuf[1024] = {0};
    char recvbuf[1024] = {0};
    while (fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)//cin
    {

        sendto(sock_cli, sendbuf, strlen(sendbuf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
		printf("client befer recvfrom\n");
        ret = recvfrom(sock_cli, recvbuf, sizeof(recvbuf), 0, NULL, NULL);
        printf("client after recvfrom\n");
        if (ret == -1)
        {
            if (errno == EINTR)
                continue;
            ERR_EXIT("recvfrom");
        }
        printf("client printf ------------\n");
        fputs(recvbuf, stdout);// cout
        memset(sendbuf, 0, sizeof(sendbuf));
        memset(recvbuf, 0, sizeof(recvbuf));
    }

    close(sock_cli);
}

int main()
{
pthread_t tida, tidb;
//pthread_mutex_init(&lock, NULL);
pthread_create(&tida, NULL, ThreadA, NULL);
pthread_create(&tidb, NULL, ThreadB, NULL);
pthread_join(tida, NULL);
pthread_join(tidb, NULL);
return 1;
}
