
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
//#include "delay.h"
//#define ERR_EXIT(m) \
        //do \
        //{ \
                //perror(m); \
                //exit(EXIT_FAILURE); \
        //} while(0)


void delay(volatile unsigned int delay)// need to add volatile!!!!!!!
{
    while(delay--);
}


int main(void)
{
    int sock;
    //if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        //ERR_EXIT("socket");
	sock = socket(PF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(14550);
    servaddr.sin_addr.s_addr = inet_addr("192.168.4.1");

    //int ret;
    //char sendbuf[1024] = {0};
    //char recvbuf[1024] = {0};
//need to add unsigned!!!!!!!
    unsigned char sendbuf_yindaomoshi[14]={0xFC,0x06,0x88,0xFF,0xBE,0x0B,0x04,0x00,0x00,0x00,0x01,0x01,0x1E,0xC5};
	unsigned char sendbuf_jiesuo[41]={0xFC,0x21,0x94,0xFF,0xBE,0x4C,0x00,0x00,0x80,0x3F,0x00,0x98,0xA5,0x46,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x90,0x01,0x01,0x01,0x00,0xE4,0xF2};
	unsigned char sendbuf_qifei14mi[41]={0xFC,0x21,0x95,0xFF,0xBE,0x4C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x41,0x16,0x00,0x01,0x01,0x00,0x56,0xC7};
	unsigned char sendbuf_jiangluo[14]={0xFC,0x06,0x5C,0xFF,0xBE,0x0B,0x09,0x00,0x00,0x00,0x01,0x01,0x5F,0x9B};

        //sendto(sock, sendbuf_yindaomoshi, strlen(sendbuf_yindaomoshi), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        //sendto(sock, sendbuf_jiesuo, strlen(sendbuf_jiesuo), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        // *******  change from strlen  to  sizeof,fianlly it's ok!!!!!!!!1
        //change delay to ----> sleep
        
	    sendto(sock, sendbuf_yindaomoshi, sizeof(sendbuf_yindaomoshi), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
	    //delay(2000000);
	    sleep(1);
        sendto(sock, sendbuf_jiesuo, sizeof(sendbuf_jiesuo), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        //delay(2000000);
        sleep(1);
        sendto(sock, sendbuf_qifei14mi, sizeof(sendbuf_qifei14mi), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        //delay(2000000);
        sleep(2);
        sendto(sock, sendbuf_jiangluo, sizeof(sendbuf_jiangluo), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
        //delay(2000000);
        sleep(1);
        /*ret = recvfrom(sock, recvbuf, sizeof(recvbuf), 0, NULL, NULL);
        if (ret == -1)
        {
            if (errno == EINTR)
                continue;
            ERR_EXIT("recvfrom");
        }

        fputs(recvbuf, stdout);
        memset(sendbuf, 0, sizeof(sendbuf));
        memset(recvbuf, 0, sizeof(recvbuf));
    */

    close(sock);// guanbuguana

    return 0;
}
