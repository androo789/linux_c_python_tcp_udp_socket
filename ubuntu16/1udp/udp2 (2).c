/*socket udo示例程序
* 由于UDP是一种对等通信，是不区分服务器端和客户端的*/

#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define SERVER_PORT 5555

int main()
{
    int peerSocket;
    struct sockaddr_in self_addr;
    struct sockaddr_in peer_addr;
    int peer_len = sizeof(peer_addr);
    int iDataNum;

    char recvbuf[200];
    char sendbuf[200];

    if((peerSocket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket");
        return 1;
    }
    bzero(&self_addr, sizeof(self_addr));
    self_addr.sin_family = AF_INET;
    self_addr.sin_port = htons(SERVER_PORT);
    self_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if(bind(peerSocket, (struct sockaddr*)&self_addr, sizeof(self_addr)) < 0)
    {
        perror("connect");
        return 1;
    }

    while(1)
    {
        printf("Input your world:>");
        scanf("%s", sendbuf);
        printf("\n");
        sendto(peerSocket, sendbuf, strlen(sendbuf), 0, (struct sockaddr*)&self_addr, sizeof(self_addr));
        iDataNum = recvfrom(peerSocket, recvbuf, 1024, 0, (struct sockaddr*)&peer_addr, (socklen_t*)&peer_len);
        recvbuf[iDataNum] = '\0';
        printf("recv from %s\n", recvbuf);
    }
    return 0;
}
