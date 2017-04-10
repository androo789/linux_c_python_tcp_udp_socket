#include <stdio.h>  
#include <iostream>  
#include <stdlib.h>  
#include <netinet/in.h>  
#include <unistd.h>  
#include <sys/socket.h>  
#include <arpa/inet.h>  
#include <string.h>  
using namespace std;  
  
#define PORT 10001  
  
int main()  
{  
    struct sockaddr_in s_in;//server address structure  
    struct sockaddr_in c_in;//client address structure  
    int l_fd,c_fd;  
    socklen_t len;  
    char buf[100];//content buff area  
    string tem;  
    float x_value = 0.0;  
    float y_value = 0.0;  
    memset((void *)&s_in,0,sizeof(s_in));  
  
    s_in.sin_family = AF_INET;//IPV4 communication domain  
    s_in.sin_addr.s_addr = INADDR_ANY;//accept any address  
    s_in.sin_port = htons(PORT);//change port to netchar  
  
    l_fd = socket(AF_INET,SOCK_STREAM,0);//socket(int domain, int type, int protocol)  
    bind(l_fd,(struct sockaddr *)&s_in,sizeof(s_in));  
    listen(l_fd,1);//同时只能有一个连接  
  
    cout<<"begin"<<endl;  
    while(1){  
        c_fd = accept(l_fd,(struct sockaddr *)&c_in,&len);  
        while(1){  
            for(int j=0;j<100;j++){  
                buf[j] = 0;  
            }  
            int n = read(c_fd,buf,100);//read the message send by client  
            if(!strcmp(buf, "q\n") || !strcmp(buf, "Q\n")){  
                cout << "q pressed\n";  
                close(c_fd);  
                break;  
            }  
            // inet_ntop(AF_INET,&c_in.sin_addr,addr_p,16);//“二进制整数” －> “点分十进制  
            tem = buf;  
            x_value = atof(tem.substr(0,5).c_str());  
            y_value = atof(tem.substr(5,5).c_str());  
            cout << x_value << "dd" << y_value <<endl;  
            write(c_fd,buf,n);//sent message back to client  
        }  
    }  
    return 0;  
}  
