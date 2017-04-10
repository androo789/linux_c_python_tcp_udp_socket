/**********************************************************
 * wu ren ji.h
 *
 *  Copy right (c) 2017, Liva Co., Ltd.
 *
 *  Created on: 2017-3-24
 *      Author: dongqi
 *     Version: v0.0.0
 * 
 *****************大无人机的指令****************************************/
    unsigned char sendbuf_yindaomoshi[14]={0xFC,0x06,0x88,0xFF,0xBE,0x0B,0x04,0x00,0x00,0x00,0x01,0x01,0x1E,0xC5};
	unsigned char sendbuf_jiesuo[41]={0xFC,0x21,0x94,0xFF,0xBE,0x4C,0x00,0x00,0x80,0x3F,0x00,0x98,0xA5,0x46,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x90,0x01,0x01,0x01,0x00,0xE4,0xF2};
	unsigned char sendbuf_qifei14mi[41]={0xFC,0x21,0x95,0xFF,0xBE,0x4C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x41,0x16,0x00,0x01,0x01,0x00,0x56,0xC7};
	unsigned char sendbuf_jiangluo[14]={0xFC,0x06,0x5C,0xFF,0xBE,0x0B,0x09,0x00,0x00,0x00,0x01,0x01,0x5F,0x9B};
	//*********************************小无人机的指令
	unsigned char takeoff_data[11]={0x04,0x0b,0x04,0x0b,0x00,0x00,0x00,0x01,0x00,0x01,0x00};
	unsigned char land_data[11]=   {0x04,0x0b,0x04,0x0b,0x00,0x00,0x00,0x01,0x00,0x03,0x00};
	unsigned char flip_left[15]=   {0x04,0x0b,0x04,0x0f,0x00,0x00,0x00,0x01,0x05,0x00,0x00,0x01,0x00,0x00,0x00};

	//parrot tcp data need to  send
	char jsonReq[]={"{\"d2c_port\":54321,\"controller_type\":\"PC\",\"controller_name\":\"bebop shell\"}"};


	//大无人机控制
	//void daWuRenJi(){
	// 改写成某个指令,‘b’是解锁，‘u’下起飞，某个指令降落‘d’，
		//*******大无人机 ，不用了
		//		if(*buf == 'b'){
		//			int sock_air;
		//			sock_air = socket(AF_INET, SOCK_DGRAM, 0);
		//			struct sockaddr_in servaddr;
		//			memset(&servaddr, 0, sizeof(servaddr));
		//			servaddr.sin_family = AF_INET;//设置ip和端口
		//			servaddr.sin_port = htons(14550);
		//			servaddr.sin_addr.s_addr = inet_addr("192.168.4.1");
		//
		//			//设置引导模式
		//			sendto(sock_air, sendbuf_yindaomoshi, sizeof(sendbuf_yindaomoshi), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
		//			sleep(5);
		//			//解锁
		//			sendto(sock_air, sendbuf_jiesuo, sizeof(sendbuf_jiesuo), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
		//			sleep(5);
		//			close(sock_air);// 应该关闭
		//			sleep(5);
		//		}else if(*buf == 'u'){
		//			int sock_air;
		//			sock_air = socket(AF_INET, SOCK_DGRAM, 0);
		//			struct sockaddr_in servaddr;
		//			memset(&servaddr, 0, sizeof(servaddr));
		//			servaddr.sin_family = AF_INET;//设置ip和端口
		//			servaddr.sin_port = htons(14550);
		//			servaddr.sin_addr.s_addr = inet_addr("192.168.4.1");
		//			sleep(5);
		//			//起飞14米
		//			sendto(sock_air, sendbuf_qifei14mi, sizeof(sendbuf_qifei14mi), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
		//			sleep(5);
		//			close(sock_air);// 应该关闭
		//			sleep(5);
		//		}else if(*buf == 'd'){
		//			int sock_air;
		//			sock_air = socket(AF_INET, SOCK_DGRAM, 0);
		//			struct sockaddr_in servaddr;
		//			memset(&servaddr, 0, sizeof(servaddr));
		//			servaddr.sin_family = AF_INET;//设置ip和端口
		//			servaddr.sin_port = htons(14550);
		//			servaddr.sin_addr.s_addr = inet_addr("192.168.4.1");
		//			sleep(5);
		//			//降落
		//			sendto(sock_air, sendbuf_jiangluo, sizeof(sendbuf_jiangluo), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
		//			sleep(5);
		//			close(sock_air);// 应该关闭
		//			sleep(5);
		//		}

	//}



	//			//########
	//			FILE *fp3;                     /*定义文件指针*/
	//			fp3=fopen("BroadCastThrd3.txt", "a");    /*建立一个文字文件只写*/
	//			fputs("recvfrom -------\n", fp3);/*向所建文件写入一串字符*/
	//			fclose(fp3);                   /*关闭文件*/
	//			//########


	//		//########
	//		FILE *fp4;                     /*定义文件指针*/
	//		fp4=fopen("BroadCastThrd4.txt", "a");    /*建立一个文字文件只写*/
	//		fputs("HERE!rcvPkt.magicId == SYS_MAGIC_ID -------\n", fp4);/*向所建文件写入一串字符*/
	//		fclose(fp4);                   /*关闭文件*/
	//		//########


	//		//########
	//		FILE *fp5;                    /*定义文件指针*/
	//		fp5=fopen("BroadCastThrd5.txt", "a");   /*建立一个文字文件只写*/
	//		fprintf(fp5, "rcvPkt.svrPort = %d,rcvPkt.udpPort =%d,pCtx->ip=%s\n"
	//				,rcvPkt.svrPort,rcvPkt.udpPort,inet_ntoa(from.sin_addr));/*向所建文件写一整型数*/
	//		fclose(fp5);                   /*关闭文件*/
	//		//########


	//			//########
	//			FILE *fp2;                     /*定义文件指针*/
	//			fp2=fopen("BroadCastThrd2.txt", "a");    /*建立一个文字文件只写*/
	//			fputs("while(1) -------\n", fp2);/*向所建文件写入一串字符*/
	//			fclose(fp2);                   /*关闭文件*/
	//			//########


	//	//########
	//	FILE *fp1;                     /*定义文件指针*/
	//	fp1=fopen("BroadCastThrd1.txt", "a");    /*建立一个文字文件只写*/
	//	fputs("BroadCastThrd-------\n", fp1);/*向所建文件写入一串字符*/
	//	fclose(fp1);                   /*关闭文件*/
	//	//########

/*

void UploadDispVal(char *buf,int size)//xxx 解调出来的数据给pc端
{//xxx 那边点一次发送按钮，这个函数被调用3次？？？？
	if(fUpSocket==-1) return;
	int tcpFlag=0;
	int				header[2];
	int				hr;
	header[0] = size;
	header[1] = CMD_SYS_DISPSTR;
	hr = SocketSnd(fUpSocket,header,8);// xxx 返回0就代表发送成功了,这里是发送”CMD_SYS_DISPSTR“头
	hr = SocketSnd(fUpSocket,buf,size);// xxx 在发送真正的数据,,buf是师兄发了3次，“文本文件”

	if((*buf == 'U' || *buf == 'D') && tcpFlag== 0){
		// tcp client socket  第一步tcp连接
				int sockParrotTcp;
				char get_msg[100] = {0};
				int len = 100,try = 5;
				int read_msg;
				struct sockaddr_in serv_addr;

				sockParrotTcp = socket(PF_INET, SOCK_STREAM, 0);
				if(sockParrotTcp == -1){
					printf(" error\n") ;
				}
				memset(&serv_addr, 0, sizeof(serv_addr));
				serv_addr.sin_family = AF_INET;
				//############ ip and port  for tcp client
				serv_addr.sin_addr.s_addr = inet_addr("192.168.42.1");  // 注释1 client
				serv_addr.sin_port = htons(44444);

				hr = SocketSnd(fUpSocket,header,8);// xxx 返回0就代表发送成功了,这里是发送”CMD_SYS_DISPSTR“头
				hr = SocketSnd(fUpSocket,'1',size);// xxx 在发送真正的数据,,buf是师兄发了3次，“文本文件”

				connect(sockParrotTcp, (struct sockaddr*) &serv_addr, sizeof(serv_addr));


				SocketSnd(sockParrotTcp,jsonReq,strlen(jsonReq));// strlen不加\0的长度
				sleep(1);
//				read_msg = SocketRcv(sockParrotTcp,get_msg,len);
				 recv(sockParrotTcp,get_msg,len,MSG_WAITALL);
				 close(sockParrotTcp);

				 tcpFlag =1;
	}else if(*buf == 'U' && tcpFlag== 1){
				int i;
				//*******************parrot无人机


				hr = SocketSnd(fUpSocket,header,8);// xxx 返回0就代表发送成功了,这里是发送”CMD_SYS_DISPSTR“头
				hr = SocketSnd(fUpSocket,'2',size);// xxx 在发送真正的数据,,buf是师兄发了3次，“文本文件”


				//第二步udp连接
				int sockParrotUdp;
				sockParrotUdp = socket(PF_INET, SOCK_DGRAM, 0);

				struct sockaddr_in servaddr;
				memset(&servaddr, 0, sizeof(servaddr));
				servaddr.sin_family = AF_INET;
				servaddr.sin_port = htons(54321);
				servaddr.sin_addr.s_addr = inet_addr("192.168.42.1");

				hr = SocketSnd(fUpSocket,header,8);// xxx 返回0就代表发送成功了,这里是发送”CMD_SYS_DISPSTR“头
				hr = SocketSnd(fUpSocket,'3',size);// xxx 在发送真正的数据,,buf是师兄发了3次，“文本文件”

					for( i = 0; i < try ;i++){
						 sendto(sockParrotUdp, takeoff_data, sizeof(takeoff_data), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
					}

				close(sockParrotUdp);


	}else if(*buf == 'D'  && tcpFlag== 1){
		int i;
		//第二步udp连接
		int sockParrotUdp;
		sockParrotUdp = socket(PF_INET, SOCK_DGRAM, 0);

		struct sockaddr_in servaddr;
		memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(54321);
		servaddr.sin_addr.s_addr = inet_addr("192.168.42.1");

		hr = SocketSnd(fUpSocket,header,8);// xxx 返回0就代表发送成功了,这里是发送”CMD_SYS_DISPSTR“头
		hr = SocketSnd(fUpSocket,'4',size);// xxx 在发送真正的数据,,buf是师兄发了3次，“文本文件”

			for( i = 0; i < try ;i++){
				 sendto(sockParrotUdp, land_data, sizeof(land_data), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
			}

		close(sockParrotUdp);
	}

}
*/
