/**********************************************************
 * TerminalIntr.c
 *
 * Ϊʲô���еĺ�����û�������أ�
 *********************************************************/
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include"wurenji.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>//ԭ��û���������ͷ�ļ�������
#include	<stdio.h>
#include	<stdlib.h>
#include 	<sys/types.h>
#include 	<sys/ioctl.h>
#include 	<sys/time.h>
#include 	<sys/stat.h>
#include     <fcntl.h>
#include	<time.h>
#include     <termios.h>

#include	<dbg_trace.h>

#include 	<unistd.h>
#include 	<sys/socket.h>
#include 	<netinet/in.h>
#include 	<netdb.h>
#include 	<errno.h>

#include	<linux/sockios.h>
#include	<linux/ethtool.h>
#include 	<linux/if.h>

#include	<ti/syslink/Std.h>
#include	<ti/syslink/IpcHost.h>
#include	<ti/syslink/SysLink.h>

#include	<ti/ipc/HeapBufMP.h>
#include 	<ti/ipc/MessageQ.h>
#include 	<ti/ipc/MultiProc.h>
#include 	<ti/ipc/Notify.h>
#include 	<ti/ipc/SharedRegion.h>
#include	<ti/sdo/linuxutils/cmem/include/cmem.h>
#include	<BurchFw.h>
#include	"BufQue.h"
#include	"mbx.h"
#include	"_UnderWater.h"
#include	"_ALGItf.h"

#define	SYS_MAGIC_ID	'UWM'

typedef	struct{
	unsigned int	magicId;
	int				svrPort;
	int				udpPort;
}BroadCastContent;

typedef	struct{
	char		ip[32];
	int			port;
	int			bdcPort;
	MBX_Handle	mbxRelease;
	MBX_Handle	mbxStart;
	int			fdDA;
	HANDLE		hQueBuf;
	int			udpPort;
	int			bUdpSnd;
	UDWEnv		*hEnv;
}TerminalIntrCtx;

typedef	struct{
	int			dwEvt;
	char		*pBuf;
	int			szBuf;
	int			szActual;
}QueBufInfo_t;

enum{
	CMD_SYS_BEEP = 0,
	CMD_SYS_CONFIG,
	CMD_SYS_DASND,
	CMD_POWER_RESET,
	CMD_SYS_STRSND,
	CMD_SYS_DISPSTR,
	CMD_SYS_UDP_START,
	CMD_SYS_UDP_STOP,
	CMD_SYS_CALIBRATE,
	CMD_SYS_SETPARAMS,
	CMD_SYS_SENDLENGTH,
	CMD_SYS_GAINPARAMS,
	CMD_TRANS_SOUND,
	CMD_TRANS_TEXT,
	};
#define	QUE_BUF_NUM			5
#define	QUE_BUF_SIZE		768000	//1s DA data: For 24KHz 76800 (Freq * 16 * 2)
static	CMEM_AllocParams	attrs = {CMEM_HEAP,CMEM_NONCACHED,128};

static	int	SocketSnd(int fd,char* buf,int size)
{
	int		hr = 0;
	int		timeout = 0;
	while(size > 0){
		hr = send(fd,buf,size,MSG_NOSIGNAL);
		if(hr <= 0){
			if(errno != EWOULDBLOCK){
				//socket error
				return	-1;
			}else{
				timeout ++;
				if(timeout > 3) return	-2;
			}
		}

		buf += hr;
		size-= hr;
	}

	return	0;
}
static	int	SocketRcv(int fd,char* buf,int size)
{
	int		hr= 0;
	int		timeout = 0;
	while(size > 0){
		hr = recv(fd,buf,size,MSG_WAITALL);
		if(hr < 0){
			if(errno != EWOULDBLOCK){
				//socket error
				return	-1;
			}else{
				//��ʱ
				timeout ++;
				if(timeout > 3){
					return	-2;
				}
			}
		}

		buf += hr;
		size-= hr;
	}

	return	0;
}

typedef struct{
	char  ip[32];
	char  mask[32];
	char  gw[32];
	char  mac[32];
	int	  sendgain;
	char  revgain;
}DevConfParams;

static	int		fUpSocket = -1;//fUpSocketȫ�ֱ���

static	void    UpUserData(unsigned int dwEvt,char* buf,int size)
{
	if(fUpSocket==-1) return;

	int				header[2];
	int				hr;
	header[0] = size;
	header[1] = dwEvt;
	hr = SocketSnd(fUpSocket,header,8);
	if(hr == 0){
		hr = SocketSnd(fUpSocket,buf,size);
	}
}
extern void	UBOOTEnv_get(char* ip,char* gw,char* mask,char* mac);
extern void	UBOOTEnv_set(char* ip,char* gw,char* mask,char* mac);
extern float	GetScaler();
extern void	SetScaler(float scaler_Val);
static	void	ParaseThrd(TerminalIntrCtx *pCtx)//xxx ���̺߳����ֿ���һ���߳�
{
	QueBufInfo_t		*pQueBuf;
//	int					nTotal = 0;

//	float 				temp;

	while(1){
		pQueBuf = BufQue_getReady(pCtx->hQueBuf,-1);

		if(pQueBuf->dwEvt == CMD_SYS_DASND){//���ָ���ǣ�da��������
			UDWDABuf	wbuf;
			wbuf.buf = (char*)CMEM_getPhys(pQueBuf->pBuf);
			wbuf.size= pQueBuf->szActual;
			CMEM_cacheWb(pQueBuf->pBuf,pQueBuf->szActual);
			write(pCtx->fdDA,(char*)&wbuf,sizeof(UDWDABuf));
		}else if(pQueBuf->dwEvt == CMD_SYS_CONFIG){
			//��ȡ�豸����
			DevConfParams *pParams = (DevConfParams*)malloc(sizeof(DevConfParams));
			UBOOTEnv_get(pParams->ip,pParams->mask,pParams->gw,pParams->mac);
			read(pCtx->hEnv->fdGain,&pParams->revgain,1);
			dbg_info("Read revgain: %d\n",pParams->revgain);
			pParams->sendgain = (int)(GetScaler()*1000);
			UpUserData(CMD_SYS_CONFIG,(char*)pParams,sizeof(DevConfParams));
			free(pParams);
		}else if(pQueBuf->dwEvt == CMD_SYS_SETPARAMS){
			//�����豸����
			DevConfParams *pParams = (DevConfParams*)pQueBuf->pBuf;
			UBOOTEnv_set(pParams->ip,pParams->mask,pParams->gw,pParams->mac);
		}else if(pQueBuf->dwEvt == CMD_SYS_GAINPARAMS){
			DevConfParams *pParams = (DevConfParams*)pQueBuf->pBuf;
			write(pCtx->hEnv->fdGain,&pParams->revgain,1);
			dbg_info("Set revgain: %d\n",pParams->revgain);
			SetScaler((float)pParams->sendgain / 1000.0);
		}
		else if(pQueBuf->dwEvt == CMD_SYS_STRSND){
			SendUserValue(pQueBuf->pBuf,pQueBuf->szActual);
		}else if(pQueBuf->dwEvt == CMD_TRANS_SOUND){
			SendUserValue(pQueBuf->pBuf,pQueBuf->szActual);
		}else if(pQueBuf->dwEvt == CMD_TRANS_TEXT){//xxx ����Ǵ��������ݿ���������ݹ������������Ҫ���Ƴ�ȥ
			SendUserValue(pQueBuf->pBuf,pQueBuf->szActual);
		}else if(pQueBuf->dwEvt == CMD_POWER_RESET){
			system("reboot");
		}else if(pQueBuf->dwEvt == CMD_SYS_UDP_START){
			pCtx->bUdpSnd = 1;
		}else if(pQueBuf->dwEvt == CMD_SYS_UDP_STOP){
			pCtx->bUdpSnd = 0;
		}else if(pQueBuf->dwEvt == CMD_SYS_CALIBRATE){
			char *buf = (char*)malloc(1024);
			pQueBuf->pBuf[pQueBuf->szActual] = 0;
			sprintf(buf,"date -s \"%s\"",pQueBuf->pBuf);
			system(buf);
			system("hwclock --systohc");
			free(buf);
		}
		CMEM_free(pQueBuf->pBuf, &attrs);
		BufQue_putReady(pCtx->hQueBuf,pQueBuf);
	}
}

typedef	struct{
	int		fdSocket;
	struct sockaddr_in addr;
	TerminalIntrCtx *pCtx;
}UdpContent;

static	UdpContent	*pUdpCtx = NULL;

static	void	UDPSndAD(TerminalIntrCtx *pCtx)//udp����ad��
{
	UdpContent *pUdp = (UdpContent*)malloc(sizeof(UdpContent));

	bzero(&pUdp->addr,sizeof(pUdp->addr));
	pUdp->addr.sin_family=AF_INET;
	pUdp->addr.sin_addr.s_addr=inet_addr(pCtx->ip);//ip

	pUdp->addr.sin_port=htons(pCtx->udpPort);//4900

	//����һ�� UDP socket

	pUdp->fdSocket=socket(AF_INET,SOCK_DGRAM,0);//IPV4  SOCK_DGRAM ���ݱ��׽��֣�UDPЭ�飩
	pUdp->pCtx = pCtx;

	pUdpCtx = pUdp;
}
void UpdSndData(char* buf,int size){
	if(pUdpCtx == NULL) return;
	if(pUdpCtx->pCtx->bUdpSnd == 0) return;

	sendto(pUdpCtx->fdSocket,buf,size,0,(struct sockaddr *)&pUdpCtx->addr,sizeof(pUdpCtx->addr));//����stop ����
}

int get_netlink_status(const char *if_name)
{
    int skfd;
    struct ifreq ifr;
    struct ethtool_value edata;
    edata.cmd = ETHTOOL_GLINK;
    edata.data = 0;
    memset(&ifr, 0, sizeof(ifr));
    strncpy(ifr.ifr_name, if_name, sizeof(ifr.ifr_name) - 1);
    ifr.ifr_data = (char *) &edata;
    if ((skfd = socket( AF_INET, SOCK_DGRAM, 0 )) == 0)
        return -1;
    if(ioctl(skfd, SIOCETHTOOL, &ifr ) == -1)
    {
        close(skfd);
        return -1;
    }
    close(skfd);
    return edata.data;
}

static	void	ConnIntrThrd(TerminalIntrCtx *pCtx)// tcp�ͻ��������߳�
{
	int					fdSocket;
	int					opval;
	struct	sockaddr_in	saddr;
	socklen_t			slen;

	struct timeval		timeo={1,0};
	int					hr,i;
	int					header[2];

	QueBufInfo_t		*pQueBuf;
	void*				ptrQueBuf[QUE_BUF_NUM];
	int					bConn = 0;
	struct	timeval		tmLast,tmCur;
	int					tmdiff;

	pthread_t			id;


	for(i = 0; i < QUE_BUF_NUM; i ++){
		pQueBuf = (QueBufInfo_t*)malloc(sizeof(QueBufInfo_t));
		pQueBuf->szBuf = QUE_BUF_SIZE;
		ptrQueBuf[i]   = pQueBuf;
	}

	pCtx->hQueBuf = BufQue_create(QUE_BUF_NUM,ptrQueBuf);

	pthread_create(&id,NULL,ParaseThrd,pCtx);//xxx �̺߳��������٣���һ���߳�,��������pCtx����Ҫ����
	pQueBuf = NULL;
	while(1){
		pUdpCtx = NULL;//���ȫ�ֱ���pUdpCtx
		MBX_pend(pCtx->mbxStart,(char*)&hr,-1);//������ס����γ��򣬱�֤�����в������̴߳��ţ������������˼
//		printf("Start connect :%s:%d\n",pCtx->ip,pCtx->port);
		UDPSndAD(pCtx);//socket��̣�����һ��udp socket��������,ȫ�ֱ���pUdpCtx,��
		//udp �ͻ��ˣ� ip=(�ұ�����һ��udp�������ˣ�˭���ҷ���Ϣ���Ҿ�֪��˭��ip��������Ǹ�ip)����������ǵ��ڱ���192.168.4.202Ҳ����127.0.0.1
		//port=4900,����udp���ݴ���˿ڣ�
reconn:
		bConn ++;
		opval = 8;
		slen  = sizeof(opval);
		if ((fdSocket = socket(AF_INET, SOCK_STREAM, 0)) < 0 ||  //tcp��SOCK_STREAM
			(setsockopt(fdSocket, IPPROTO_IP, IP_TOS, &opval, slen) < 0)){//xxx ���ð������ȼ���2017-3-29 15:30#######################################################################################
			goto connExit;
		}

		//���ý��պͷ��ͳ�ʱ
		setsockopt(fdSocket, SOL_SOCKET, SO_SNDTIMEO, &timeo, sizeof(struct timeval));
		setsockopt(fdSocket, SOL_SOCKET, SO_RCVTIMEO, &timeo, sizeof(struct timeval));

		saddr.sin_family = AF_INET;
		saddr.sin_port = htons((unsigned short)pCtx->port);//xxx  ���Թ㲥�߳̽��յ������ݣ�6490
		saddr.sin_addr.s_addr = inet_addr(pCtx->ip);

		if (connect(fdSocket, (struct sockaddr*) &saddr, sizeof(saddr)) == -1){//xxx �����tcp�ͻ���
			if(++bConn > 3) goto connExit;

			close(fdSocket);
			dbg_info("Connect time out.\n");
			goto reconn;
		}
		gettimeofday(&tmLast,NULL);//��ȡ��ǰʱ��

		fUpSocket = fdSocket;//xxx fUpSocketȫ�ֱ��������︳ֵ,��tcp�ͻ���
		while(1){
			gettimeofday(&tmCur,NULL);
			tmdiff = tmCur.tv_sec - tmLast.tv_sec;
			if((tmdiff > 5) || (tmdiff < 0)){// �������ִ�е�ʱ����0-5΢��֮��
				header[0] = 0;
				header[1] = CMD_SYS_BEEP;

				hr = get_netlink_status("eth0");
				if(hr != 1){
//					dbg_info("Disconnect!!!!!!!!\n");
					break;
				}

				hr = SocketSnd(fdSocket,(char*)header,8);
				if(hr == -1){
					break;
				}
				gettimeofday(&tmLast,NULL);
			}
			if(pQueBuf == NULL){
				pQueBuf = BufQue_getFree(pCtx->hQueBuf,500);
				continue;
			}
			memset(header,0,8);
			hr = recv(fdSocket,header,8,MSG_WAITALL);//xxx ��������
			if(hr <= 0){
				if(errno != EWOULDBLOCK){
					break;
				}
				continue;
			}
			gettimeofday(&tmLast,NULL);

			pQueBuf->dwEvt    = header[1];//���¼���������������¼���ParaseThrd�߳��ﴦ��
			pQueBuf->szActual = header[0] > pQueBuf->szBuf ? pQueBuf->szBuf : header[0];
			if(pQueBuf->szActual > 0)
				pQueBuf->pBuf  = (unsigned int)CMEM_alloc(pQueBuf->szActual,&attrs);
			else
				pQueBuf->pBuf  = (unsigned int)CMEM_alloc(1,&attrs);

			hr = SocketRcv(fdSocket,pQueBuf->pBuf,pQueBuf->szActual);//xxx ��������
			if(hr == -1){
				break;
			}

			BufQue_putFree(pCtx->hQueBuf,pQueBuf);//��ֵ���������
			pQueBuf = NULL;
		}

connExit:
		fUpSocket = -1;
		if(fdSocket >=0) close(fdSocket);
		MBX_post(pCtx->mbxRelease,(char*)&hr,500);
		dbg_info("MBX_post mbxRelease\n");
	}

}

//void parrot(char flag){//flag = 'U' ��ɣ�flag ='D'  ���䣬
//
//		int i;
//		//*******************parrot���˻�
////	// tcp client socket  ��һ��tcp����
//		int sockParrotTcp;
//		char get_msg[100] = {0};
//////		    int result, opnd_cnt, i;
//		int len = 100,try = 5;
//		int read_msg;
//		struct sockaddr_in serv_addr;
//
//		sockParrotTcp = socket(PF_INET, SOCK_STREAM, 0);
//		if(sockParrotTcp == -1){
//			printf(" error\n") ;
//		}
//		memset(&serv_addr, 0, sizeof(serv_addr));
//		serv_addr.sin_family = AF_INET;
//		//############ ip and port  for tcp client
//		serv_addr.sin_addr.s_addr = inet_addr("192.168.42.1");  // ע��1 client
//		serv_addr.sin_port = htons(44444);
//
//		connect(sockParrotTcp, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
////
//		//data need to  send
//		char jsonReq[]={"{\"d2c_port\":54321,\"controller_type\":\"PC\",\"controller_name\":\"bebop shell\"}"};
//
////
////
//		SocketSnd(sockParrotTcp,jsonReq,strlen(jsonReq));// strlen?
//		sleep(1);
//		read_msg = SocketRcv(sockParrotTcp,get_msg,len);
//		//		int read_msg = read(sockParrotTcp, get_msg, len);
////
////
//		close(sockParrotTcp);
//
//
//		//�ڶ���udp����
////		printf("udp transmit start\n");
//		int sockParrotUdp;
//		sockParrotUdp = socket(PF_INET, SOCK_DGRAM, 0);
////		if () < 0)
////			printf("udpconn_process socket build error!\n");
//
//		struct sockaddr_in servaddr;
//		memset(&servaddr, 0, sizeof(servaddr));
//		servaddr.sin_family = AF_INET;
//		servaddr.sin_port = htons(54321);
//		servaddr.sin_addr.s_addr = inet_addr("192.168.42.1");
//		if(flag == 'U'){
//			for( i = 0; i < try ;i++){
//				 sendto(sockParrotUdp, takeoff_data, sizeof(takeoff_data), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
////				if(dongqi <= 0){
////					fputs("sendto fail! -------\n", fp5);/*�������ļ�д��һ���ַ�*/
////				}else{
////					fputs("sendto  ok ! -------\n", fp5);/*�������ļ�д��һ���ַ�*/
////				}
//			}
//		}else if (flag == 'D'){
//			for( i = 0; i < try ;i++){
//				 sendto(sockParrotUdp, land_data, sizeof(land_data), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
////				if(dongqi <= 0){
////					fputs("sendto fail! -------\n", fp5);/*�������ļ�д��һ���ַ�*/
////				}else{
////					fputs("sendto  ok ! -------\n", fp5);/*�������ļ�д��һ���ַ�*/
////				}
//			}
//		}
//
//
////			if(sendto(sockParrotUdp, land_data, sizeof(land_data), 0, (struct sockaddr *)&servaddr, sizeof(servaddr))<0){
//		//xxx must use sizeof instead of strlen,don't mislead by python
////				printf("sendto error ...\n");
////			}
//		close(sockParrotUdp);
//
//
//}



void UploadDispVal(char *buf,int size)//xxx ������������ݸ�pc��
{//xxx �Ǳߵ�һ�η��Ͱ�ť���������������3�Σ�������
	if(fUpSocket==-1) return;
	int try = 5;
	static int flag = 0;
	int				header[2];
	int				hr;
	header[0] = size;
	header[1] = CMD_SYS_DISPSTR;
	hr = SocketSnd(fUpSocket,header,8);// xxx ����0�ʹ����ͳɹ���,�����Ƿ��͡�CMD_SYS_DISPSTR��ͷ

	hr = SocketSnd(fUpSocket,buf,size);// xxx �ڷ�������������,,buf��ʦ�ַ���3�Σ����ı��ļ���


	//UDP Process
	if(*buf == 'A'){
		struct sockaddr_in serv_addr;
		int i;
		int sockParrotUdp;

		serv_addr.sin_addr.s_addr = inet_addr("192.168.42.1");
		serv_addr.sin_family = PF_INET;


		char get_msg[100] = {0};
		int len = 100;
		char jsonReq[]={"{\"d2c_port\":54321,\"controller_type\":\"PC\",\"controller_name\":\"bebop shell\"}"};
		sockParrotUdp = socket(PF_INET, SOCK_STREAM, 0);
		serv_addr.sin_port = htons(44444);

		connect(sockParrotUdp, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
		SocketSnd(sockParrotUdp,jsonReq,strlen(jsonReq));
		sleep(0.5);
		recv(sockParrotUdp,get_msg,len,MSG_WAITALL);
		close(sockParrotUdp);

		sockParrotUdp = socket(PF_INET, SOCK_DGRAM, 0);
		serv_addr.sin_port = htons(54321);
		if (*(buf+1) == 'L')
		{
			for( i = 0; i < try ;i++){
				 sendto(sockParrotUdp, flip_left, sizeof(flip_left), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
			}
		}
		else if(flag == 0)
		{
			for( i = 0; i < try ;i++){
				 sendto(sockParrotUdp, takeoff_data, sizeof(takeoff_data), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
			}
			flag = flag + 1;
		}else
		{
			for( i = 0; i < try ;i++){
				 sendto(sockParrotUdp, land_data, sizeof(land_data), 0, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
			}
			flag = flag - 1;
		}

		close(sockParrotUdp);


	}//else if(*buf == 'T'){//����tcp���ӣ�����֮��udp����
//		// tcp client socket  ��һ��tcp����
//						int sockParrotTcp;
//						char get_msg[100] = {0};
//						int len = 100;
//						int read_msg;
//						struct sockaddr_in serv_addr;
//
//						sockParrotTcp = socket(PF_INET, SOCK_STREAM, 0);
//						if(sockParrotTcp == -1){
//							printf(" error\n") ;
//						}
//						memset(&serv_addr, 0, sizeof(serv_addr));
//						serv_addr.sin_family = AF_INET;
//						//############ ip and port  for tcp client
//						serv_addr.sin_addr.s_addr = inet_addr("192.168.42.1");  // ע��1 client
//						serv_addr.sin_port = htons(44444);
//
//						connect(sockParrotTcp, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
//
//						//data need to  send
//						char jsonReq[]={"{\"d2c_port\":54321,\"controller_type\":\"PC\",\"controller_name\":\"bebop shell\"}"};
//
//						SocketSnd(sockParrotTcp,jsonReq,strlen(jsonReq));// strlen?
//						sleep(1);
//		//				read_msg = SocketRcv(sockParrotTcp,get_msg,len);
//						recv(sockParrotTcp,get_msg,len,MSG_WAITALL);
//						close(sockParrotTcp);
//	}
//
}

static	void	BroadCastThrd(TerminalIntrCtx *pCtx)//xxx ������udp�߳�,��Ϊ��s_addr.sin_addr.s_addr	= INADDR_ANY;SOCK_DGRAM�ȵȴ���
{//xxx �㲥�߳� ������������ã�����rcvPkt��ȷ����magicId���id���õ�svrPort��udpPort��pCtx����
	//xxx ��Ӧ�Ķ�Ӧ�ĳ���Ӧ����   �ͻ���udp��̣���ֵmagicId��svrPort��udpPort




	int						sock;
	struct	sockaddr_in		s_addr,from;
	int						hr,from_len;
	BroadCastContent		rcvPkt;
//	pthread_t				id;
//	printf("Broad Cast Thread start...\n");
	while(1){


		if((sock = socket(AF_INET,SOCK_DGRAM,0)) == -1){// xxx udp����SOCK_DGRAM��udp�㲥
//			printf("Create socket error.\n");
//			sleep(5);
			continue;
		}
		dbg_info("BroadCastThrd\n");

		memset(&s_addr,0,sizeof(struct sockaddr_in));
		s_addr.sin_family 		= AF_INET;
		s_addr.sin_port			= htons(pCtx->bdcPort);
		s_addr.sin_addr.s_addr	= INADDR_ANY;
		bind(sock,(struct sockaddr*)&s_addr,sizeof(s_addr));

		while(1){
			from_len = sizeof(from);
			hr = recvfrom(sock,&rcvPkt,sizeof(BroadCastContent),0,(struct sockaddr*)&from,&from_len);
			//xxx udp���������������Ϣ




			if(hr < -1) break;
			if(hr != sizeof(BroadCastContent)) continue;
			if(rcvPkt.magicId != SYS_MAGIC_ID) continue;
			//������ѭ��ͷ��ʼִ�У���recvfrom����....,����ɹ������Ǹ�SYS_MAGIC_ID���͹رգ����˳�ѭ��
			close(sock);
			break;
		}


		// �ӹ㲥�߳����÷����豸��ip������˿�svrPort 6490 �����ݶ˿�udpPort 4900
		sprintf(pCtx->ip,"%s",inet_ntoa(from.sin_addr));//����ip����������Ǵ�ӡ���ݵ��ַ���pCtx->ip
		//����������from�ͻ�õ�����������������ģ��ͻ��˵�ip��ַ
		pCtx->port = rcvPkt.svrPort;//svrPortv 6490,�������ö˿ںţ�����
		pCtx->udpPort = rcvPkt.udpPort;//udpPort 4900



//		printf("ConnInfo:%s:%d\n",pCtx->ip,pCtx->port);
		MBX_post(pCtx->mbxStart,(char*)&hr,-1);
		//Ϊʲô���������û�ж���
		MBX_pend(pCtx->mbxRelease,(char*)&hr,-1);
		dbg_info("MBX_pend mbxRelease!!!!!\n");//xxx ��仰��������������ʱ����ô��̵��Եģ�


	}//���while�ǲ��ǲ��������,�����á�a�����ļ�����ôû��һֱ�������ݣ�
	//BroadCastThrd2�ظ�6�飬BroadCastThrd2�ظ�5��
	//�����ԣ����whileִ��5�Σ��ļ������ظ�5�飬��������Ϊ�Ǳ� �Է��ͷ���100�飬
	//�������ֻ���ܵ�5�飬Ȼ����������recvfrom����ط���

}

//static	void	SelfConnThrdBroadcast(TerminalIntrCtx *pCtx)//xxx �Լ������λ���Ĺ��ܣ�������λ��������100��㲥������������ť��������
//{
//	sleep(10);//xxx ����ͣһ�ᣬ�ñ���߳���ִ�е���������״̬
////�ļ�����,ͨ���ļ�����
//	FILE *fp;                     /*�����ļ�ָ��*/
//    fp=fopen("SelfConnThrd.txt", "w");    /*����һ�������ļ�ֻд*/
//
//
//	int broadCastSock;
//	if ((broadCastSock = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
//		fputs("socket fail-------", fp);/*�������ļ�д��һ���ַ�*/
//	}
//
//
//    //���ù㲥
//	const int opt = 1;
//    if(setsockopt(broadCastSock, SOL_SOCKET, SO_BROADCAST, (char *)&opt, sizeof(opt)) == -1)
//    {
//    	fputs("set broadcast  fail-------", fp);/*�������ļ�д��һ���ַ�*/
//    }
//
//    struct sockaddr_in servaddr;
//    memset(&servaddr, 0, sizeof(servaddr));
//    servaddr.sin_family = AF_INET;
//    servaddr.sin_port = htons(4009);
//    servaddr.sin_addr.s_addr = inet_addr("192.168.4.255");
//
//	BroadCastContent	sndCtx;//xxx ������Ǹ������㲥�������͵����� ��������
//	sndCtx.magicId = 'UWM';//#define	SYS_MAGIC_ID	'UWM'
//	sndCtx.svrPort = 6490;//����˿ں�6490
//	sndCtx.udpPort = 4009;//���ݶ˿ںţ����Ƕ�Ӧ������ģ�4009
//
//	int		count = 0;
//	while(count++ < 100){
//		sendto(broadCastSock,(char*)&sndCtx,sizeof(BroadCastContent),0,(struct sockaddr*)&servaddr,sizeof(servaddr));
//	}
//	close(broadCastSock);
//	fputs("close socket-------", fp);/*�������ļ�д��һ���ַ�*/
//	fclose(fp);                   /*�ر��ļ�*/
//}

//static	void	SelfConnThrdTcpServer(TerminalIntrCtx *pCtx)//xxx �Լ������λ���Ĺ��ܣ�������λ��������100��㲥������������ť��������
//{
//	sleep(10);//xxx ����ͣһ�ᣬ�ñ���߳���ִ�е���������״̬
//
//}



void	TerminalIntr_init(UDWEnv *hEnv,int fdDA,int bdcPort)//bdcPort=4009
{
	TerminalIntrCtx	*pCtx;
	pthread_t		id;

	pCtx = (TerminalIntrCtx*)malloc(sizeof(TerminalIntrCtx));
	pCtx->mbxRelease = MBX_create(1,sizeof(int));
	pCtx->mbxStart   = MBX_create(1,sizeof(int));
	pCtx->hEnv       = hEnv;
	pCtx->fdDA = fdDA;//DA����
	pCtx->bdcPort = bdcPort;//bdcPort=4009

	//pCtx�ǹ����������Ҫ���������ܷ��ʡ�
	pthread_create(&id,NULL,BroadCastThrd,pCtx);//��udp�������߳�
	pthread_create(&id,NULL,ConnIntrThrd,pCtx);//
//	pthread_create(&id,NULL,SelfConnThrdBroadcast,pCtx);//�Լ����͹㲥�߳�
//	pthread_create(&id,NULL,SelfConnThrdTcpServer,pCtx);//�Լ����͹㲥�߳�
}
