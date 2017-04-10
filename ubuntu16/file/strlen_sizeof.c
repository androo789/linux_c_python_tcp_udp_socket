
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>


int main(int argc, char *argv[])
{
//    char jsonReq[100];
//    char jsonReq[] ={"d2c_port":54321,"controller_type":"PC","controller_name":"bebop shell"}
//    sprintf(jsonReq,"%s","d2c_port:54321,controller_type:PC,controller_name:bebop shell");
    char jsonReq[]={"abcde"};
    printf(" jsonReq = %s\n",jsonReq);
    printf("strlen =%d\n",strlen(jsonReq));
    printf("sizeof = %d\n",sizeof(jsonReq));




unsigned char takeoff_data[11]={0x04,0x0b,0x04,0x0b,0x00,0x00,0x00,0x01,0x00,0x01,0x00};
printf("strlen =  %d\n",strlen(takeoff_data));
printf("sizeof = %d\n",sizeof(takeoff_data));

    return 0;
}
