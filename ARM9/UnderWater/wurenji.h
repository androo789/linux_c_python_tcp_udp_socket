/**********************************************************
 * wu ren ji.h
 *
 *  Copy right (c) 2017, Liva Co., Ltd.
 *
 *  Created on: 2017-3-24
 *      Author: dongqi
 *     Version: v0.0.0
 * 
 *****************�����˻���ָ��****************************************/
    unsigned char sendbuf_yindaomoshi[14]={0xFC,0x06,0x88,0xFF,0xBE,0x0B,0x04,0x00,0x00,0x00,0x01,0x01,0x1E,0xC5};
	unsigned char sendbuf_jiesuo[41]={0xFC,0x21,0x94,0xFF,0xBE,0x4C,0x00,0x00,0x80,0x3F,0x00,0x98,0xA5,0x46,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x90,0x01,0x01,0x01,0x00,0xE4,0xF2};
	unsigned char sendbuf_qifei14mi[41]={0xFC,0x21,0x95,0xFF,0xBE,0x4C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x60,0x41,0x16,0x00,0x01,0x01,0x00,0x56,0xC7};
	unsigned char sendbuf_jiangluo[14]={0xFC,0x06,0x5C,0xFF,0xBE,0x0B,0x09,0x00,0x00,0x00,0x01,0x01,0x5F,0x9B};
	//*********************************С���˻���ָ��
	unsigned char takeoff_data[11]={0x04,0x0b,0x04,0x0b,0x00,0x00,0x00,0x01,0x00,0x01,0x00};
	unsigned char land_data[11]=   {0x04,0x0b,0x04,0x0b,0x00,0x00,0x00,0x01,0x00,0x03,0x00};
	unsigned char flip_left[15]=   {0x04,0x0b,0x04,0x0f,0x00,0x00,0x00,0x01,0x05,0x00,0x00,0x01,0x00,0x00,0x00};

	//parrot tcp data need to  send
	char jsonReq[]={"{\"d2c_port\":54321,\"controller_type\":\"PC\",\"controller_name\":\"bebop shell\"}"};


	//�����˻�����
	//void daWuRenJi(){
	// ��д��ĳ��ָ��,��b���ǽ�������u������ɣ�ĳ��ָ��䡮d����
		//*******�����˻� ��������
		//		if(*buf == 'b'){
		//			int sock_air;
		//			sock_air = socket(AF_INET, SOCK_DGRAM, 0);
		//			struct sockaddr_in servaddr;
		//			memset(&servaddr, 0, sizeof(servaddr));
		//			servaddr.sin_family = AF_INET;//����ip�Ͷ˿�
		//			servaddr.sin_port = htons(14550);
		//			servaddr.sin_addr.s_addr = inet_addr("192.168.4.1");
		//
		//			//��������ģʽ
		//			sendto(sock_air, sendbuf_yindaomoshi, sizeof(sendbuf_yindaomoshi), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
		//			sleep(5);
		//			//����
		//			sendto(sock_air, sendbuf_jiesuo, sizeof(sendbuf_jiesuo), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
		//			sleep(5);
		//			close(sock_air);// Ӧ�ùر�
		//			sleep(5);
		//		}else if(*buf == 'u'){
		//			int sock_air;
		//			sock_air = socket(AF_INET, SOCK_DGRAM, 0);
		//			struct sockaddr_in servaddr;
		//			memset(&servaddr, 0, sizeof(servaddr));
		//			servaddr.sin_family = AF_INET;//����ip�Ͷ˿�
		//			servaddr.sin_port = htons(14550);
		//			servaddr.sin_addr.s_addr = inet_addr("192.168.4.1");
		//			sleep(5);
		//			//���14��
		//			sendto(sock_air, sendbuf_qifei14mi, sizeof(sendbuf_qifei14mi), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
		//			sleep(5);
		//			close(sock_air);// Ӧ�ùر�
		//			sleep(5);
		//		}else if(*buf == 'd'){
		//			int sock_air;
		//			sock_air = socket(AF_INET, SOCK_DGRAM, 0);
		//			struct sockaddr_in servaddr;
		//			memset(&servaddr, 0, sizeof(servaddr));
		//			servaddr.sin_family = AF_INET;//����ip�Ͷ˿�
		//			servaddr.sin_port = htons(14550);
		//			servaddr.sin_addr.s_addr = inet_addr("192.168.4.1");
		//			sleep(5);
		//			//����
		//			sendto(sock_air, sendbuf_jiangluo, sizeof(sendbuf_jiangluo), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
		//			sleep(5);
		//			close(sock_air);// Ӧ�ùر�
		//			sleep(5);
		//		}

	//}



	//			//########
	//			FILE *fp3;                     /*�����ļ�ָ��*/
	//			fp3=fopen("BroadCastThrd3.txt", "a");    /*����һ�������ļ�ֻд*/
	//			fputs("recvfrom -------\n", fp3);/*�������ļ�д��һ���ַ�*/
	//			fclose(fp3);                   /*�ر��ļ�*/
	//			//########


	//		//########
	//		FILE *fp4;                     /*�����ļ�ָ��*/
	//		fp4=fopen("BroadCastThrd4.txt", "a");    /*����һ�������ļ�ֻд*/
	//		fputs("HERE!rcvPkt.magicId == SYS_MAGIC_ID -------\n", fp4);/*�������ļ�д��һ���ַ�*/
	//		fclose(fp4);                   /*�ر��ļ�*/
	//		//########


	//		//########
	//		FILE *fp5;                    /*�����ļ�ָ��*/
	//		fp5=fopen("BroadCastThrd5.txt", "a");   /*����һ�������ļ�ֻд*/
	//		fprintf(fp5, "rcvPkt.svrPort = %d,rcvPkt.udpPort =%d,pCtx->ip=%s\n"
	//				,rcvPkt.svrPort,rcvPkt.udpPort,inet_ntoa(from.sin_addr));/*�������ļ�дһ������*/
	//		fclose(fp5);                   /*�ر��ļ�*/
	//		//########


	//			//########
	//			FILE *fp2;                     /*�����ļ�ָ��*/
	//			fp2=fopen("BroadCastThrd2.txt", "a");    /*����һ�������ļ�ֻд*/
	//			fputs("while(1) -------\n", fp2);/*�������ļ�д��һ���ַ�*/
	//			fclose(fp2);                   /*�ر��ļ�*/
	//			//########


	//	//########
	//	FILE *fp1;                     /*�����ļ�ָ��*/
	//	fp1=fopen("BroadCastThrd1.txt", "a");    /*����һ�������ļ�ֻд*/
	//	fputs("BroadCastThrd-------\n", fp1);/*�������ļ�д��һ���ַ�*/
	//	fclose(fp1);                   /*�ر��ļ�*/
	//	//########

/*

void UploadDispVal(char *buf,int size)//xxx ������������ݸ�pc��
{//xxx �Ǳߵ�һ�η��Ͱ�ť���������������3�Σ�������
	if(fUpSocket==-1) return;
	int tcpFlag=0;
	int				header[2];
	int				hr;
	header[0] = size;
	header[1] = CMD_SYS_DISPSTR;
	hr = SocketSnd(fUpSocket,header,8);// xxx ����0�ʹ����ͳɹ���,�����Ƿ��͡�CMD_SYS_DISPSTR��ͷ
	hr = SocketSnd(fUpSocket,buf,size);// xxx �ڷ�������������,,buf��ʦ�ַ���3�Σ����ı��ļ���

	if((*buf == 'U' || *buf == 'D') && tcpFlag== 0){
		// tcp client socket  ��һ��tcp����
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
				serv_addr.sin_addr.s_addr = inet_addr("192.168.42.1");  // ע��1 client
				serv_addr.sin_port = htons(44444);

				hr = SocketSnd(fUpSocket,header,8);// xxx ����0�ʹ����ͳɹ���,�����Ƿ��͡�CMD_SYS_DISPSTR��ͷ
				hr = SocketSnd(fUpSocket,'1',size);// xxx �ڷ�������������,,buf��ʦ�ַ���3�Σ����ı��ļ���

				connect(sockParrotTcp, (struct sockaddr*) &serv_addr, sizeof(serv_addr));


				SocketSnd(sockParrotTcp,jsonReq,strlen(jsonReq));// strlen����\0�ĳ���
				sleep(1);
//				read_msg = SocketRcv(sockParrotTcp,get_msg,len);
				 recv(sockParrotTcp,get_msg,len,MSG_WAITALL);
				 close(sockParrotTcp);

				 tcpFlag =1;
	}else if(*buf == 'U' && tcpFlag== 1){
				int i;
				//*******************parrot���˻�


				hr = SocketSnd(fUpSocket,header,8);// xxx ����0�ʹ����ͳɹ���,�����Ƿ��͡�CMD_SYS_DISPSTR��ͷ
				hr = SocketSnd(fUpSocket,'2',size);// xxx �ڷ�������������,,buf��ʦ�ַ���3�Σ����ı��ļ���


				//�ڶ���udp����
				int sockParrotUdp;
				sockParrotUdp = socket(PF_INET, SOCK_DGRAM, 0);

				struct sockaddr_in servaddr;
				memset(&servaddr, 0, sizeof(servaddr));
				servaddr.sin_family = AF_INET;
				servaddr.sin_port = htons(54321);
				servaddr.sin_addr.s_addr = inet_addr("192.168.42.1");

				hr = SocketSnd(fUpSocket,header,8);// xxx ����0�ʹ����ͳɹ���,�����Ƿ��͡�CMD_SYS_DISPSTR��ͷ
				hr = SocketSnd(fUpSocket,'3',size);// xxx �ڷ�������������,,buf��ʦ�ַ���3�Σ����ı��ļ���

					for( i = 0; i < try ;i++){
						 sendto(sockParrotUdp, takeoff_data, sizeof(takeoff_data), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
					}

				close(sockParrotUdp);


	}else if(*buf == 'D'  && tcpFlag== 1){
		int i;
		//�ڶ���udp����
		int sockParrotUdp;
		sockParrotUdp = socket(PF_INET, SOCK_DGRAM, 0);

		struct sockaddr_in servaddr;
		memset(&servaddr, 0, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(54321);
		servaddr.sin_addr.s_addr = inet_addr("192.168.42.1");

		hr = SocketSnd(fUpSocket,header,8);// xxx ����0�ʹ����ͳɹ���,�����Ƿ��͡�CMD_SYS_DISPSTR��ͷ
		hr = SocketSnd(fUpSocket,'4',size);// xxx �ڷ�������������,,buf��ʦ�ַ���3�Σ����ı��ļ���

			for( i = 0; i < try ;i++){
				 sendto(sockParrotUdp, land_data, sizeof(land_data), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
			}

		close(sockParrotUdp);
	}

}
*/
