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

typedef struct {
unsigned char type;
unsigned char buf;
unsigned char seq;
int size;
}parrot;




void* udpconn_process(void* arg)
{
    printf("udp transmit start\n");
	int sock;
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        printf("udpconn_process socket build error!\n");

	struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(54321);
    servaddr.sin_addr.s_addr = inet_addr("192.168.42.1");


	unsigned char takeoff_data[11]={0x04,0x0b,0x04,0x0b,0x00,0x00,0x00,0x01,0x00,0x01,0x00};
	unsigned char land_data[11]=   {0x04,0x0b,0x04,0x0b,0x00,0x00,0x00,0x01,0x00,0x03,0x00};

    if(sendto(sock, land_data, sizeof(land_data), 0, (struct sockaddr *)&servaddr, sizeof(servaddr))<0){
    //must use sizeof instead of strlen,don't mislead by python
        printf("sendto error ...\n");
    }
	close(sock);
}

void* udpconn_listener(void* arg)
{
    printf("udp listen star\n");
    char recvbuf[1024]={0};
    char the_data[1024]={0};
    struct sockaddr_in peer_addr;
    int peer_len = sizeof(peer_addr);
    int iDataNum;

	int recv_sock;
    if ((recv_sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
		printf("udpconn_listener socket build error!\n");
	}


    struct sockaddr_in self_addr;
    bzero(&self_addr, sizeof(self_addr));
    self_addr.sin_family = AF_INET;
    self_addr.sin_port = htons(54321);
    self_addr.sin_addr.s_addr = inet_addr("0.0.0.0");

    if(bind(recv_sock, (struct sockaddr*)&self_addr, sizeof(self_addr)) <0 ){
		printf("bind error!\n");
	}


	int alive =1;
	while(alive == 1){

    iDataNum =recvfrom(recv_sock, recvbuf, 1024, 0, (struct sockaddr*)&peer_addr, (socklen_t*)&peer_len);
//	recvbuf[iDataNum] = '\0';
    printf("iDataNum = %d\n",iDataNum);
    printf("strlen  recvfrom = %d\n",strlen(recvfrom));
    printf("sizeof recvfrom = %d\n",sizeof(recvfrom));

    printf("recv from1: %d\n", *recvbuf);
    printf("recv from2: %d\n", *(recvbuf+1));
    printf("recv from3: %d\n", *(recvbuf+2));
    printf("recv from4: %d\n", *(recvbuf+3));
    printf("recv from5: %d\n", *(recvbuf+4));
    printf("recv from6: %d\n", *(recvbuf+5));
    printf("recv from7: %d\n", *(recvbuf+6));
    printf("recv from8: %d\n", *(recvbuf+7));


	}



}





int main(int argc, char *argv[])
{
    // tcp client socket
    int sock;
    char get_msg[4096] = {0};
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
    printf("get_msg = %s\n",get_msg);

    close(sock);


    int res;
    pthread_t thread1,thread2;
    printf("udp process start ...\n");
    //begin udpconn_process
    res = pthread_create(&thread1, NULL, udpconn_process, NULL);
    if (res != 0)
    {
        perror("Thread1 creation failed!");
        exit(EXIT_FAILURE);
    }

    //begin udpconn_listener
//    res = pthread_create(&thread2, NULL, udpconn_listener, NULL);
//
//    if (res != 0)
//    {
//        perror("Thread2 creation failed!");
//        exit(EXIT_FAILURE);
//    }
    while(1);//!!!!!!!!
    return 0;
}



