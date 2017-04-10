/**********************************************************
 * DecFsk.c
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
#include	<ti/sdo/linuxutils/cmem/include/cmem.h>
#include	<liva/dsp/DecFsk/DecFsk.h>
#include	<liva/common/sout/sout.h>
#include	"_ALGItf.h"
#include	"mbx.h"


typedef	struct{
	UDWEnv			*hEnv;

	PALGIntf		hDecAlg;
	PDECFSKParam	pDecParams;

	PALGIntf		hSoutAlg;
	PSOUTParam		pSoutParam;

	int				fd;

	MBX_Handle		mbxFree;
	MBX_Handle		mbxReady;
	PCROSSBuf		pCrossBuf;
	char			*dispBuf;
}DECFskEnv;

static	CMEM_AllocParams	attrs = {CMEM_HEAP,CMEM_NONCACHED,128};

extern void UploadDispVal(char *buf,int size);
static	int	fxnSoutCB(PBASParam param,void* cbArg,void* content)
{//xxx 那边点一次发送按钮，接收这边这个函数被调用3次？？？？
	unsigned char	*pBuf = (unsigned char*)content;
	DECFskEnv		*decEnv = (DECFskEnv*)cbArg;

	CMEM_cacheInv(pBuf,28);//xxx 学习cache,解调出来的数据实际上是从cache中拿来的
	//printf("Call Back:%08x:%02X,%02X,%02x,%02x..\n",content,pBuf[0],pBuf[1],pBuf[2],pBuf[3]);
	memcpy(decEnv->dispBuf,pBuf,28);
	pBuf[28] = 0;

	UploadDispVal(decEnv->dispBuf,28);//xxx 解调出来的数据给pc端
//	printf("\nRecv:\033[1;31m%s\033[0m\n",decEnv->dispBuf);
}
#if 0
static	void	DecFskThrd(DECFskEnv *hEnv)
{
	UDWBuf_t			buf_t;
	short				*pSrc;
	int					i,szBuf;
	FILE				*fp = fopen("ad.dat","wb");
	printf("DecFskThrd...\n");
	while(1){
		MBX_pend(hEnv->mbxReady,(char*)&buf_t,-1);

		CMEM_cacheWb(buf_t.pVirt,buf_t.actual);
		fwrite(buf_t.pVirt,buf_t.actual,1,fp);
		hEnv->pDecParams->pInData = buf_t.pPhys;//CMEM_getPhys((void*)pBuf);
		hEnv->pDecParams->szInData= buf_t.actual;
		hEnv->hDecAlg->fxnProcess(hEnv->hDecAlg,0);

		pSrc = (short*)buf_t.pVirt;
	//	printf("%04X\n",pSrc[0] );
		MBX_post(hEnv->mbxFree,(char*)&buf_t,-1);
	}
}
static	void	ADThrd(DECFskEnv *hEnv)
{
	int			fd;
	UDWBuf_t	buf_t;

	fd = open("/dev/ADSample",O_RDWR);
	while(1){
		MBX_pend(hEnv->mbxFree,(char*)&buf_t,-1);

		buf_t.actual = read(fd,buf_t.pVirt,buf_t.size);

		MBX_post(hEnv->mbxReady,(char*)&buf_t,-1);
	}
}
#else
#include	"ADSample.h"
static	void	DecFskThrd(DECFskEnv *hEnv)
{
	BufInfo_t			buf_t,tmpBuf;

	int					bWait = 0;

	while(1){

		ioctl(hEnv->fd,AD_CMD_LOCK,&buf_t);
/*
		bWait = 0;
		if(hEnv->hEnv->bSave){
			MBX_post(hEnv->mbxReady,(char*)&buf_t,-1);
			bWait = 1;
		}
		*/
		MBX_post(hEnv->mbxReady,(char*)&buf_t,-1);
		hEnv->pDecParams->pInData = buf_t.pPhys;//CMEM_getPhys((void*)pBuf);
		hEnv->pDecParams->szInData= buf_t.actual;
		if(hEnv->hEnv->bSending == 0){
			hEnv->hDecAlg->fxnProcess(hEnv->hDecAlg,0);
		}
		/*
		if(bWait){
			MBX_pend(hEnv->mbxFree,(char*)&tmpBuf,-1);
		}*/
		MBX_pend(hEnv->mbxFree,(char*)&tmpBuf,-1);
		ioctl(hEnv->fd,AD_CMD_UNLOCK,&buf_t);
	}
}

extern void UpdSndData(char* buf,int size);
static	void	ADThrd(DECFskEnv *hEnv)
{
	BufInfo_t			buf_t;
	char				*buf = NULL;
	int					szBuf = 0;
	unsigned short		*pSrc;

	while(1){
		MBX_pend(hEnv->mbxReady,(char*)&buf_t,-1);
		if(szBuf < buf_t.actual){
			if(buf != NULL) free(buf);
			szBuf = buf_t.actual + 1024;
			buf   = (char*)malloc(szBuf);
		}
		buf_t.pPhys = buf;
		buf_t.size= buf_t.actual;

		ioctl(hEnv->fd,AD_CMD_COPY,&buf_t);
		MBX_post(hEnv->mbxFree,(char*)&buf_t,-1);

#if 0
		if(hEnv->hEnv->bSave){
			fwrite(buf,buf_t.size,1,hEnv->hEnv->fp);
			fflush(hEnv->hEnv->fp);
		}else{
			fclose(hEnv->hEnv->fp);
			hEnv->hEnv->fp = NULL;
		}
#endif
		//UDP 发送
		UpdSndData(buf,buf_t.size);
	}

}
#endif
void	DECFsk_init(UDWEnv *hEnv)
{
	DECFskEnv		*decEnv;
	pthread_t		id;

	int				i;
	UDWBuf_t		buf_t;
	CROSSBuf		outBuf;

	decEnv = (char*)malloc(sizeof(DECFskEnv));
	decEnv->hEnv       = hEnv;
	decEnv->dispBuf    = (char*)malloc(128);

	decEnv->hSoutAlg   = SOUT_create();
	decEnv->pSoutParam = (PSOUTParam)((PMSGObj)decEnv->hSoutAlg->param.virt)->msgBuf;
	decEnv->pSoutParam->cbArg = decEnv;
	decEnv->pSoutParam->fxnProcCB = fxnSoutCB;//xxx 那边点一次发送按钮，接收这边这个函数被调用3次？？？？

	decEnv->hDecAlg = DECFSK_create();
	decEnv->pDecParams = (PDECFSKParam)((PMSGObj)decEnv->hDecAlg->param.virt)->msgBuf;

	decEnv->pDecParams->nSplite   = 240*16;
	decEnv->pDecParams->szOutData = 58;
	decEnv->pDecParams->nDataArray= 10;


	outBuf.virt = (char*)CMEM_alloc(decEnv->pDecParams->szOutData*decEnv->pDecParams->nDataArray,&attrs);
	outBuf.phys = CMEM_getPhys(outBuf.virt);
	decEnv->pCrossBuf = (PCROSSBuf)CMEM_alloc(sizeof(CROSSBuf)*decEnv->pDecParams->nDataArray,&attrs);
	for(i = 0; i < decEnv->pDecParams->nDataArray; i ++){
		decEnv->pCrossBuf[i].virt = outBuf.virt + i * decEnv->pDecParams->szOutData;
		decEnv->pCrossBuf[i].phys = outBuf.phys + i * decEnv->pDecParams->szOutData;
	//	printf("[%d]:%08X->%08x\n",i,decEnv->pCrossBuf[i].virt,decEnv->pCrossBuf[i].phys);
	}
	CMEM_cacheWb(decEnv->pCrossBuf,sizeof(CROSSBuf)*decEnv->pDecParams->nDataArray);

	decEnv->pDecParams->outDataArray = (PCROSSBuf)CMEM_getPhys(decEnv->pCrossBuf);

	sprintf(decEnv->pDecParams->base.name,"DECFSK");
	CODECS_rpeDebug(decEnv->hDecAlg,FALSE);

	decEnv->hDecAlg->fxnCreate(decEnv->hDecAlg);
	CODECS_nodeLink(decEnv->hDecAlg,decEnv->hSoutAlg,ALG_FUNC_PROCESS);
//	printf("Create OK:%d.\n",hEnv->nCapBuf);

	decEnv->mbxFree  = MBX_create(hEnv->nCapBuf+2,sizeof(BufInfo_t));
	decEnv->mbxReady = MBX_create(hEnv->nCapBuf+2,sizeof(BufInfo_t));
	decEnv->fd = open("/dev/ADSample",O_RDWR);

	pthread_create(&id,NULL,ADThrd,decEnv);
	pthread_create(&id,NULL,DecFskThrd,decEnv);
}
