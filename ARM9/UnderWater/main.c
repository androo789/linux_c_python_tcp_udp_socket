/**********************************************************
 * main.c
 *
 * 
 *********************************************************/
#include	<stdio.h>
#include	<stdlib.h>
#include 	<sys/types.h>
#include 	<sys/stat.h>
#include 	<stdio.h>
#include 	<fcntl.h>
#include 	<sys/time.h>
#include 	<sys/types.h>
#include 	<unistd.h>
#include 	<stdlib.h>
#include 	<time.h>
#include 	<linux/ioctl.h>

#include	<ti/syslink/Std.h>
#include	<ti/syslink/IpcHost.h>
#include	<ti/syslink/SysLink.h>

#include	<ti/ipc/HeapBufMP.h>
#include 	<ti/ipc/MessageQ.h>
#include 	<ti/ipc/MultiProc.h>
#include 	<ti/ipc/Notify.h>
#include 	<ti/ipc/SharedRegion.h>

#include	<BurchFw.h>
#include	<AlgStub.h>
#include	<ti/xdais/dm/xdm.h>
#include	<dbg_trace.h>

#include	"_ALGItf.h"

static	UDWEnv env;

static	void LogInInfo()
{
	printf("\033[1;31m+---------------------------------------------+\033[0m\n");
	printf("\033[1;31m|                                             |\033[0m\n");
	printf("\033[1;31m|    UnderWater Communication System V1.0     |\033[0m\n");
	printf("\033[1;31m|                                             |\033[0m\n");
	printf("\033[1;31m+---------------------------------------------+\033[0m\n");

}
void	main(int argc,char* argv[])
{
	UDWEnv		*hEnv = &env;
	void		*h;

	hEnv->nCapBuf = 5;
	hEnv->szBuf   = 7680;//15360;

	hEnv->enc_scaler = 0.1;//8191;
	hEnv->enc_szOutData = 153600;
	hEnv->enc_szPkt     = 28;//43;

	hEnv->bSave = 0;
	hEnv->fp    = NULL;
	CMEM_init();
	CODECS_install(NULL);

//	LogInInfo();

	hEnv->bSending = 0;
	hEnv->mbxSendWait = MBX_create(1,sizeof(int));
	if(argc < 2 ){
		unsigned char		val=0;
		hEnv->fdGain = open("/dev/AD5246",O_RDWR);
		write(hEnv->fdGain,&val,1);

		DECFsk_init(hEnv);
		h = ENCFsk_init(hEnv);

		while(1){
			ENCFsk_test(h);
		}
	}else if(atoi(argv[1]) == 1){
		//±àÂë
		EncFskDemo(hEnv,argc,argv);
	}else if(atoi(argv[1]) == 2){
		DecFskDemo(hEnv,argc,argv);
	}else{
		printf("Usage:\n");
		printf("\t[opt<0/1>] [fname]\n");
	}
}
