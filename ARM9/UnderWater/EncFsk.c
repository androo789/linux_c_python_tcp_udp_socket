/**********************************************************
 * EncFsk.c
 *
 * 
 *********************************************************/
#include	<stdlib.h>
#include 	<sys/types.h>
#include 	<sys/stat.h>
#include 	<stdio.h>
#include 	<fcntl.h>
#include 	<sys/time.h>
#include 	<sys/types.h>
#include 	<unistd.h>
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
#include	<ti/sdo/linuxutils/cmem/include/cmem.h>
#include	<liva/dsp/EncFsk/EncFsk.h>
#include	<liva/common/sout/sout.h>

#include	"_ALGItf.h"
#include	"mbx.h"

typedef	struct{
	unsigned char	*buf;
	int				size;
}UDWDABuf;

typedef	struct{
	UDWEnv		*hEnv;

	PALGIntf		hEncAlg;
	PENCFSKParam	pEncParams;

	PALGIntf		hSoutAlg;
	PSOUTParam		pSoutParam;

	PCROSSBuf		pCrossBuf;

	int				fd;
}ENCFskEnv;


static	CMEM_AllocParams	attrs = {CMEM_HEAP,CMEM_NONCACHED,128};
static	void	taskSendState(UDWEnv* hEnv)
{
	int		val,hr;
	while(1){
		if(hEnv->bSending == 0){
			MBX_pend(hEnv->mbxSendWait,(char*)&val,MBX_WAIT_FOREVER);
		}else{
			hr = MBX_pend(hEnv->mbxSendWait,(char*)&val,1000);
			if(hr == -1){
				hEnv->bSending = 0;
			}
		}
	}
}
#if 1
static	int	fxnSoutCB(PBASParam param,void* cbArg,void* content)
{
	ENCFskEnv	*encEnv = (ENCFskEnv*)cbArg;
	PCROSSBuf	pCrossBuf;
	int			idx = (int)content;
	UDWDABuf	wbuf;
	int				val = 0;
	unsigned short	*pSrc;

	pCrossBuf = &encEnv->pCrossBuf[idx];
	//printf("Call Back:%08x->%08X..\n",pCrossBuf->phys,pCrossBuf->virt);

	pSrc = (unsigned short*)pCrossBuf->virt;
	//*pSrc = 0xD001;
	wbuf.buf = (char*)pCrossBuf->phys;
	wbuf.size= encEnv->pEncParams->szOutData;
	write(encEnv->fd,(char*)&wbuf,sizeof(UDWDABuf));
	MBX_post(encEnv->hEnv->mbxSendWait,(char*)&val,MBX_WAIT_FOREVER);
}
#else
#if 0
static	int	fxnSoutCB(PBASParam param,void* cbArg,void* content)
{
	ENCFskEnv	*encEnv = (ENCFskEnv*)cbArg;
	PCROSSBuf	pCrossBuf;
	int			idx = (int)content;
	UDWDABuf	wbuf;
	int			hr;
	static	char	*pTestMem = NULL;
	static	FILE	*fp = NULL;
	if(fp == NULL){
		fp = fopen("enc.dat","rb");
	}
	if(pTestMem == NULL){
		pTestMem  = (char*)CMEM_alloc(encEnv->pEncParams->szOutData,&attrs);
	}
	pCrossBuf = &encEnv->pCrossBuf[idx];
	printf("Call Back:%08x->%08X,%d..\n",pCrossBuf->phys,pCrossBuf->virt,idx);

	pCrossBuf->virt = pTestMem;
	pCrossBuf->phys = CMEM_getPhys(pTestMem);

	hr = fread((char*)pCrossBuf->virt,encEnv->pEncParams->szOutData,1,fp);
	if(hr <= 0){
		fseek(fp,0,SEEK_SET);
		hr = fread((char*)pCrossBuf->virt,encEnv->pEncParams->szOutData,1,fp);
	//	printf("Read again.\n");
	}
	CMEM_cacheWbInv((char*)pCrossBuf->virt,encEnv->pEncParams->szOutData);
	{
		char	*pPtr = (char*)pCrossBuf->virt;
	//	printf("%02X,%02X,%02X,%02X\n",pPtr[0],pPtr[1],pPtr[2],pPtr[3]);
	}


	wbuf.buf = (char*)pCrossBuf->phys;
	wbuf.size= encEnv->pEncParams->szOutData;
	write(encEnv->fd,(char*)&wbuf,sizeof(UDWDABuf));
}
#else
#include <math.h>
void	GenSin(short *buf,int len)
{
	short		val;
	int				i;
	for(i = 0; i < len; i ++){
		val = 2047 * sinf((i%16)*2*3.1415926/16)+2048;

		val = (4<<12)|(val&0xFFF);
		*buf++ = val;

	}
}
static	int	fxnSoutCB(PBASParam param,void* cbArg,void* content)
{
	ENCFskEnv	*encEnv = (ENCFskEnv*)cbArg;
	PCROSSBuf	pCrossBuf;
	int			idx = (int)content;
	UDWDABuf	wbuf;
	int			hr;

	pCrossBuf = &encEnv->pCrossBuf[idx];
	printf("Call Back:%08x->%08X,%d..\n",pCrossBuf->phys,pCrossBuf->virt,idx);

	GenSin(pCrossBuf->virt,encEnv->pEncParams->szOutData/2);
	CMEM_cacheWbInv((char*)pCrossBuf->virt,encEnv->pEncParams->szOutData);

	wbuf.buf = (char*)pCrossBuf->phys;
	wbuf.size= encEnv->pEncParams->szOutData;
	write(encEnv->fd,(char*)&wbuf,sizeof(UDWDABuf));
}
#endif
#endif
void*	ENCFsk_init(UDWEnv *hEnv)
{
	ENCFskEnv		*encEnv;

	int				i;
	UDWBuf_t		buf_t;
	CROSSBuf		outBuf;

	encEnv = (char*)malloc(sizeof(ENCFskEnv));
	encEnv->hEnv       = hEnv;

	encEnv->hSoutAlg   = SOUT_create();
	encEnv->pSoutParam = (PSOUTParam)((PMSGObj)encEnv->hSoutAlg->param.virt)->msgBuf;
	encEnv->pSoutParam->cbArg = encEnv;
	encEnv->pSoutParam->fxnProcCB = fxnSoutCB;

	encEnv->hEncAlg = DECFSK_create();
	encEnv->pEncParams = (PENCFSKParam)((PMSGObj)encEnv->hEncAlg->param.virt)->msgBuf;

	encEnv->pEncParams->scaler   = hEnv->enc_scaler;//8191;
	encEnv->pEncParams->nSplite  = hEnv->enc_szPkt;//43;
	encEnv->pEncParams->szOutData= hEnv->enc_szOutData * 2;//153600*2;
	encEnv->pEncParams->nDataArray= 10;


	encEnv->pCrossBuf = (PCROSSBuf)CMEM_alloc(sizeof(CROSSBuf)*encEnv->pEncParams->nDataArray,&attrs);
	for(i = 0; i < encEnv->pEncParams->nDataArray; i ++){
		encEnv->pCrossBuf[i].virt = CMEM_alloc(encEnv->pEncParams->szOutData,&attrs);
		encEnv->pCrossBuf[i].phys = CMEM_getPhys((void*)encEnv->pCrossBuf[i].virt);
	//	printf("[%d]:%08X->%08x\n",i,encEnv->pCrossBuf[i].virt,encEnv->pCrossBuf[i].phys);
	}
	CMEM_cacheWb(encEnv->pCrossBuf,sizeof(CROSSBuf)*encEnv->pEncParams->nDataArray);

	encEnv->pEncParams->outDataArray = (PCROSSBuf)CMEM_getPhys(encEnv->pCrossBuf);

	sprintf(encEnv->pEncParams->base.name,"ENCFSK");
	CODECS_rpeDebug(encEnv->hEncAlg,FALSE);

	encEnv->hEncAlg->fxnCreate(encEnv->hEncAlg);
	CODECS_nodeLink(encEnv->hEncAlg,encEnv->hSoutAlg,ALG_FUNC_PROCESS);
	//printf("Create OK:%d.\n",hEnv->nCapBuf);

	//Open DA
	encEnv->fd = open("/dev/UdwDA",O_RDWR);// 由此看出da在这上面已经是一个驱动了，他也没给da驱动程序

	TerminalIntr_init(hEnv,encEnv->fd,4009);//广播端口4009
	return	encEnv;
}
#if 0
void	ENCFsk_test(void *h)
{
	ENCFskEnv		*encEnv = (ENCFskEnv*)h;
	char			*pBuf;
	int				szBuf;

	int				size;
	int				val,i;

	szBuf = 15360;
	pBuf  = (char*)CMEM_alloc(szBuf,&attrs);
	while(1){
		//
		printf("Input Size:");
		scanf("%d",&size);
		printf("Input Value:");
		scanf("%d",&val);
#if 0
		if(size > szBuf){
			CMEM_free(pBuf,&attrs);
			szBuf = size + 1024;
			pBuf  = (char*)CMEM_alloc(szBuf,&attrs);
		}
#else
		if(size>szBuf) size = szBuf;
#endif
		for(i = 0; i < size; i ++){
			//memset(pBuf,val,size);
			pBuf[i] = val&0xff;
			val = val+1;
		}
		CMEM_cacheWb(pBuf,size);
		encEnv->pEncParams->pInData = (char*)CMEM_getPhys(pBuf);
		encEnv->pEncParams->szInData= size;
		encEnv->hEncAlg->fxnProcess(encEnv->hEncAlg,0);
	}
}
#else
static	void	HelpUsage()
{
	printf("\nCommand:\n");
	printf("\t Usage:@[CMD]:\n");
	printf("\t\t@SetScale=[float Value]\n");
	printf("\t\t@SetGain=[Hex Value]\n");
	printf("\t\t@GetGain\n");
	printf("\t\t@Save=[file path]\n");
	printf("\t\t@Stop\n");
}
typedef	struct{
	ENCFskEnv		*encEnv;
	char*			pBuf;
	int				szBuf;
}UserEncCtx;

static	UserEncCtx*		gUserEncCtx=NULL;

float	GetScaler()
{
	if(gUserEncCtx == NULL) return 0;
	return gUserEncCtx->encEnv->pEncParams->scaler;
}

void	SetScaler(float scaler_Val)
{
	if(gUserEncCtx == NULL) return 0;
	gUserEncCtx->encEnv->pEncParams->scaler = scaler_Val;
}

void	ENCFsk_test(void *h)
{
	ENCFskEnv		*encEnv = (ENCFskEnv*)h;
	char			*pBuf,*pPtr;
	int				szBuf;

	int				size;
	int				val,i;
	pthread_t		id;

	pthread_create(&id,NULL,taskSendState,encEnv->hEnv);
	szBuf = 15360;
//	pBuf  = (char*)CMEM_alloc(szBuf,&attrs);

	gUserEncCtx = (UserEncCtx*)malloc(sizeof(UserEncCtx));
	gUserEncCtx->encEnv = encEnv;
	gUserEncCtx->szBuf  = 15360;
	gUserEncCtx->pBuf   = (char*)CMEM_alloc(gUserEncCtx->szBuf,&attrs);

	while(1){
		sleep(1);
#if 0
		//
		memset(pBuf,0x0,szBuf);
		printf("Input String:");
		scanf("%s",pBuf);
		//gets(pBuf);

		if(pBuf[0] == '@')
		{
			pPtr = strstr(pBuf,"@SetScale=");
			if(pPtr != NULL){
				pPtr = pBuf + strlen("@SetScale=");
				encEnv->pEncParams->scaler = atof(pPtr);
				printf("SetScale:%.5f\n",encEnv->pEncParams->scaler);
				continue;
			}

			pPtr = strstr(pBuf,"@SetGain=");
			if(pPtr != NULL){
				unsigned char	gain;
				pPtr = pBuf + strlen("@SetGain=");
				sscanf(pPtr,"%x",&val);

				val = val < 0 ? 0 : val;
				val = val > 0x7F ? 0x7F : val;
				gain = val;
				write(encEnv->hEnv->fdGain,&gain,1);

				printf("SetGain:%02X\n",gain);
				continue;
			}

			pPtr = strstr(pBuf,"@GetGain");
			if(pPtr != NULL){
				unsigned char	gain;
				read(encEnv->hEnv->fdGain,&gain,1);
				printf("GetGain:%02X\n",gain);
				continue;
			}

			pPtr = strstr(pBuf,"@Save=");
			if(pPtr != NULL){
				pPtr = pBuf + strlen("@Save=");
				encEnv->hEnv->fp = fopen(pPtr,"wb");
				encEnv->hEnv->bSave = 1;
				continue;
			}


			pPtr = strstr(pBuf,"@Stop");
			if(pPtr != NULL){
				encEnv->hEnv->bSave = 0;
				continue;
			}

			if(strcmp(pBuf,"@Help") == 0){
				HelpUsage();
				continue;
			}
			if(strcmp(pBuf,"@Quit") == 0){
				system("reboot -f");
				continue;
			}
		}
		size = strlen(pBuf);
		size = ((size+encEnv->hEnv->enc_szPkt-1)/encEnv->hEnv->enc_szPkt) * encEnv->hEnv->enc_szPkt;
		if(size>szBuf) size = szBuf;

		CMEM_cacheWb(pBuf,size);
		encEnv->hEnv->bSending = 1;
		encEnv->pEncParams->pInData = (char*)CMEM_getPhys(pBuf);
		encEnv->pEncParams->szInData= size;
		encEnv->hEncAlg->fxnProcess(encEnv->hEncAlg,0);
#endif
	}

//	CMEM_free(pBuf,&attrs);
}

void	SendUserValue(char* pVal,int szVal)
{
	if(gUserEncCtx == NULL) return;
	ENCFskEnv		*encEnv = gUserEncCtx->encEnv;
	char			*pBuf = gUserEncCtx->pBuf;
	int				size,szActual;

	size = szVal;
	size = ((size+encEnv->hEnv->enc_szPkt-1)/encEnv->hEnv->enc_szPkt) * encEnv->hEnv->enc_szPkt;
	if(size>gUserEncCtx->szBuf) size = gUserEncCtx->szBuf;

	szActual = szVal > size ? size : szVal;
	memcpy(pBuf,pVal,szActual);
	memset(pBuf+szActual,0,size - szActual);


	CMEM_cacheWb(pBuf,size);
	encEnv->hEnv->bSending = 1;
	encEnv->pEncParams->pInData = (char*)CMEM_getPhys(pBuf);
	encEnv->pEncParams->szInData= size;
	encEnv->hEncAlg->fxnProcess(encEnv->hEncAlg,0);
}
#endif
