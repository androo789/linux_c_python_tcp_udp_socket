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
#include"delay.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define SERVER_PORT 5555

int main()
{
    int peerSocket;
    struct sockaddr_in self_addr;
    //struct sockaddr_in peer_addr;
    //int peer_len = sizeof(peer_addr);
    //int iDataNum;

    //char recvbuf[200];
    //char sendbuf[200];

    char sendbuf_yindaomoshi[50]={0xFC,0x06,0x88,0xFF,0xBE,0x0B,0x04,0x00,0x00,0x00,0x01,0x01,0x1E,0xC5};
	char sendbuf_jiesuo[50]={0xFC,0x21,0x94,0xFF,0xBE,0x4C,0x00,0x00,0x80,0x3F,0x00,0x98,0xA5,0x46,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x90,0x01,0x01,0x01,0x00,0xE4,0xF2};
	char sendbuf_qifei14mi[50]={0xFC,0x21,0x95,0xFF,0xBE,0x4C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x41,0x16,0x00,0x01,0x01,0x00,0x56,0xC7};
	char sendbuf_jiangluo[50]={0xFC,0x06,0x5C,0xFF,0xBE,0x0B,0x09,0x00,0x00,0x00,0x01,0x01,0x5F,0x9B};


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

    /*while(1)
    {
        printf("Input your world:>");
        scanf("%s", sendbuf);
        printf("\n");
        sendto(peerSocket, sendbuf, strlen(sendbuf), 0, (struct sockaddr*)&self_addr, sizeof(self_addr));
        iDataNum = recvfrom(peerSocket, recvbuf, 1024, 0, (struct sockaddr*)&peer_addr, (socklen_t*)&peer_len);
        recvbuf[iDataNum] = '\0';
        printf("recv from %s\n", recvbuf);
    }*/
	sendto(peerSocket, sendbuf_yindaomoshi, strlen(sendbuf_yindaomoshi), 0, (struct sockaddr*)&self_addr, sizeof(self_addr));
	delay(100);
	sendto(peerSocket, sendbuf_jiesuo, strlen(sendbuf_jiesuo), 0, (struct sockaddr*)&self_addr, sizeof(self_addr));
	delay(100);
	sendto(peerSocket, sendbuf_qifei14mi, strlen(sendbuf_qifei14mi), 0, (struct sockaddr*)&self_addr, sizeof(self_addr));
	delay(100);
	sendto(peerSocket, sendbuf_jiangluo, strlen(sendbuf_jiangluo), 0, (struct sockaddr*)&self_addr, sizeof(self_addr));

    return 0;
}
