#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
    // tcp client socket
    int sock;
    char get_msg[1000000] = {0};
    int result, opnd_cnt, i;
    int len;
    struct sockaddr_in serv_addr;

    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1){
        return -1;
    }
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
//    //############ ip and port  for tcp client
    serv_addr.sin_addr.s_addr = inet_addr("192.168.42.1");  // 注释1 client
    serv_addr.sin_port = htons(44444);

    if(connect(sock, (struct sockaddr*) &serv_addr, sizeof(serv_addr))==-1){ // 注释2
        printf("connect error\n") ;
        return -1;
    }
    else{
        printf("connected ...\n");  //注释3
    }
//
//
//    //data need to  send
    char jsonReq[]={"{\"d2c_port\":54321,\"controller_type\":\"PC\",\"controller_name\":\"bebop shell\"}"};


    len = write(sock, jsonReq, strlen(jsonReq));
    int read_msg = read(sock, get_msg, len);

    printf("read_msg  = %d\n",read_msg);
    printf("len  = %d\n",len);
    printf("sizeof get_msg  = %d\n",sizeof(get_msg));
    printf("strlen get_msg  = %d\n",strlen(get_msg));
    printf("get_msg = %s\n",get_msg);

    printf("get_msg = %c\n",get_msg[74]);
    printf("get_msg = %c\n",get_msg[75]);
    printf("get_msg = %c\n",get_msg[76]);
    printf("get_msg = %c\n",get_msg[77]);
    printf("get_msg = %c\n",get_msg[78]);
    printf("get_msg = %c\n",get_msg[79]);
    printf("get_msg = %c\n",get_msg[80]);


    close(sock);

////////////////////////////////////////


    printf("udp process start ...\n");

    printf("udp transmit start\n");
	int sock2;
    if ((sock2 = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        printf("udpconn_process socket build error!\n");

	struct sockaddr_in servaddr2;
    memset(&servaddr2, 0, sizeof(servaddr2));
    servaddr2.sin_family = AF_INET;
    servaddr2.sin_port = htons(54321);
    servaddr2.sin_addr.s_addr = inet_addr("192.168.42.1");


	unsigned char takeoff_data[11]=    {0x04,0x0b,0x04,0x0b,0x00,0x00,0x00,0x01,0x00,0x01,0x00};
	unsigned char land_data[11]=       {0x04,0x0b,0x04,0x0b,0x00,0x00,0x00,0x01,0x00,0x03,0x00};
	unsigned char xiangna_data[20]=    {0x02,0x0a,0x00,0x14,0x00,0x00,0x00,0x01,0x00,0x02,0x00,0x01,0x0a,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    unsigned char tingxia_data[20]=    {0x02,0x0a,0x00,0x14,0x00,0x00,0x00,0x01,0x00,0x02,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
    printf(" sizeof =  %d\n",sizeof(takeoff_data));

    if(sendto(sock2, takeoff_data, sizeof(takeoff_data), 0, (struct sockaddr *)&servaddr2, sizeof(servaddr2))<0){
    //must use sizeof instead of strlen,don't mislead by python
        printf("sendto error ...\n");
    }


    close(sock2);

    return 0;
}



