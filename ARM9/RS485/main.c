/**********************************************************
 * main.c
 *
 * 
 *********************************************************/
#include	<sys/types.h>
#include	<sys/stat.h>
#include	<stdio.h>
#include	<fcntl.h>
#include	<sys/time.h>
#include	<sys/types.h>
#include	<unistd.h>
#include	<stdlib.h>
#include	<linux/ioctl.h>
#include 	<sys/ioctl.h>
#include 	<sys/time.h>
#include 	<sys/stat.h>
#include	<time.h>
#include	<termios.h>
#include 	<string.h>

#include	"BufQue.h"

static void setTermios(struct termios * pNewtio, int uBaudRate)
{
    bzero(pNewtio, sizeof(struct termios)); /* clear struct for new port settings */
    //8N1
    pNewtio->c_cflag = uBaudRate | CS8 | CREAD | CLOCAL;
    pNewtio->c_iflag = IGNPAR;
    pNewtio->c_oflag = 0;
    pNewtio->c_lflag = 0; //non ICANON

    pNewtio->c_cc[VINTR] = 0; /* Ctrl-c */
    pNewtio->c_cc[VQUIT] = 0; /* Ctrl-\ */
    pNewtio->c_cc[VERASE] = 0; /* del */
    pNewtio->c_cc[VKILL] = 0; /* @ */
    pNewtio->c_cc[VEOF] = 4; /* Ctrl-d */
    pNewtio->c_cc[VTIME] = 5; /* inter-character timer, timeout VTIME*0.1 */
    pNewtio->c_cc[VMIN] = 0; /* blocking read until VMIN character arrives */
    pNewtio->c_cc[VSWTC] = 0; /* '\0' */
    pNewtio->c_cc[VSTART] = 0; /* Ctrl-q */
    pNewtio->c_cc[VSTOP] = 0; /* Ctrl-s */
    pNewtio->c_cc[VSUSP] = 0; /* Ctrl-z */
    pNewtio->c_cc[VEOL] = 0; /* '\0' */
    pNewtio->c_cc[VREPRINT] = 0; /* Ctrl-r */
    pNewtio->c_cc[VDISCARD] = 0; /* Ctrl-u */
    pNewtio->c_cc[VWERASE] = 0; /* Ctrl-w */
    pNewtio->c_cc[VLNEXT] = 0; /* Ctrl-v */
    pNewtio->c_cc[VEOL2] = 0; /* '\0' */
}

typedef	struct{
	int			size;
	int			szActual;
	pthread_mutex_t		mutex;
	char		buf[0];
}SocUartRxBuf;

typedef	struct{
	int					fd;
	BufQue_Handle		hRxQueBuf;
}SocConnCtx;

#define	QUE_BUF_NUM		10
#define	QUE_BUF_SIZE	1024

void Cmdtest(SocUartRxBuf* dataBuf) {
	dataBuf->buf[dataBuf->szActual] = 0;
	printf("%s\n",dataBuf->buf);
}

void CmdProc(SocUartRxBuf* dataBuf) {
	int				fd;
	unsigned char	*ptr;
	unsigned int	size;

	ptr = dataBuf->buf;
	size = dataBuf->szActual;

	while(((ptr[0] != 'l') || (ptr[1] != 't')) && (size > 0)){
		ptr ++;
		size --;
	}

	if(size) {
		ptr[size] = 0;
		//TODO:Add Process Code
		printf("%s\n", &ptr);
	}
}

void UartParaseThrd(SocConnCtx *pCtx)
{
	SocUartRxBuf	*pRxBuf, *dataBuf;

	dataBuf = (SocUartRxBuf*)malloc(QUE_BUF_SIZE + sizeof(SocUartRxBuf));
	dataBuf->size = QUE_BUF_SIZE;
	dataBuf->szActual = 0;

	while(1){
		pRxBuf = (SocUartRxBuf*)BufQue_getReady(pCtx->hRxQueBuf,-1);

		memcpy(dataBuf->buf, pRxBuf->buf, pRxBuf->szActual);
		dataBuf->szActual = pRxBuf->szActual;

		pRxBuf->szActual = 0;
		BufQue_putReady(pCtx->hRxQueBuf,pRxBuf);

		CmdProc(dataBuf);
	}
}
void UartRxThrd(SocConnCtx *pCtx)
{
	struct timeval 	tv;
	int				timeout;
	fd_set 			rfds;

	SocUartRxBuf	*pRxBuf = NULL;

	while(1){
		if(pRxBuf == NULL){
			pRxBuf = (SocUartRxBuf*)BufQue_getFree(pCtx->hRxQueBuf,-1);
			pRxBuf->szActual = 0;
		}
		tv.tv_sec =0;
		tv.tv_usec=500000;

		FD_ZERO(&rfds);
		FD_SET(pCtx->fd, &rfds);

		timeout = select(1+pCtx->fd, &rfds, NULL, NULL, &tv);

		if(timeout > 0)
		{
		   if (FD_ISSET(pCtx->fd, &rfds))
		   {
			  pRxBuf->szActual += read(pCtx->fd, pRxBuf->buf + pRxBuf->szActual, pRxBuf->size - pRxBuf->szActual);
		   }
		}else if((timeout == 0) && (pRxBuf->szActual != 0))
		{
			BufQue_putFree(pCtx->hRxQueBuf,pRxBuf);
			pRxBuf = NULL;
		}
	}
}

void main(int argc,char* argv[])
{
	SocConnCtx		socCtx;
	SocConnCtx		*pCtx = &socCtx;
	struct termios 		oldtio, newtio;
	pthread_t		id;
	SocUartRxBuf	*pRxBuf;

	int				i;
	void*			ptrQueBuf[QUE_BUF_NUM];
	char			*devPath = argc > 1 ? argv[1] : "/dev/ttyS1";

	char *rs485 = "RS485!!";

	pCtx->fd = open(devPath,O_RDWR | O_NOCTTY);

	tcgetattr(pCtx->fd, &oldtio); /* save current serial port settings */
	setTermios(&newtio, B115200);
	tcflush(pCtx->fd, TCIFLUSH);
	tcsetattr(pCtx->fd, TCSANOW, &newtio);


	for(i = 0; i < QUE_BUF_NUM; i++){
		pRxBuf = (SocUartRxBuf*)malloc(QUE_BUF_SIZE + sizeof(SocUartRxBuf));
		pRxBuf->size = QUE_BUF_SIZE;
		pRxBuf->szActual = 0;
		ptrQueBuf[i] = (void*)pRxBuf;
	}
	pCtx->hRxQueBuf = BufQue_create(QUE_BUF_NUM,ptrQueBuf);


	pthread_create(&id,NULL,UartRxThrd,pCtx);
	pthread_create(&id,NULL,UartParaseThrd,pCtx);

	while(1){
		//TODO:Write
		write(pCtx->fd, rs485, strlen(rs485));
		sleep(1);
	}
}
