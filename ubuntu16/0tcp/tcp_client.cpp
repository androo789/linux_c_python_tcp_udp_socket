#include <stdio.h>  
#include <string.h>  
#include <iostream>  
#include <stdlib.h>  
#include <string>  
#include <unistd.h>  
#include <arpa/inet.h>  
#include <sys/socket.h>  
#define BUF_SIZE 100  
#define ADDR "127.0.0.1" //在本机测试用这个地址，如果连接其他电脑需要更换IP  
#define SERVERPORT 10001  
using namespace std;  
int main(int argc, char *argv[])  
{  
    int sock;  
    char opmsg[BUF_SIZE];  
    char get_msg[BUF_SIZE] = {0};  
    int result, opnd_cnt, i;  
    int len;  
    bool if_first = true;  
    struct sockaddr_in serv_addr;  
  
    sock = socket(PF_INET, SOCK_STREAM, 0);  
    if(sock == -1){  
        return -1;  
    }  
    memset(&serv_addr, 0, sizeof(serv_addr));  
    serv_addr.sin_family = AF_INET;  
    serv_addr.sin_addr.s_addr = inet_addr(ADDR);  // 注释1  
    serv_addr.sin_port = htons(SERVERPORT);  
    if(connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1){ // 注释2  
        cout << "connect error\n";  
        return -1;  
    }  
    else{  
        cout << "connected ...\n" << endl;  //注释3  
    }  
  
    while(1){  
        fgets(opmsg, BUF_SIZE, stdin);          
        len = write(sock, opmsg, strlen(opmsg)); // 注释4  
        if(!strcmp(opmsg, "q\n") || !strcmp(opmsg, "Q\n"))  
        {  
            puts("q pressed\n");  
            break;  
        }  
        else  
        {  
            int read_msg = read(sock, get_msg, len);  
            cout << "send length:" << len << " get:" << get_msg << endl;  
        }  
    }  
    close(sock);  
    return 0;  
}  
