/**********************************************************
 * DecFskDemo.c
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
}DECFskEnv;

static	CMEM_AllocParams	attrs = {CMEM_HEAP,CMEM_CACHED,128};

static	int	fxnSoutCB(PBASParam param,void* cbArg,void* content)
{
	unsigned char	*pBuf = (unsigned char*)content;
	printf("Call Back:%08x:%02X,%02X..\n",content,pBuf[0],pBuf[1]);

}

static	void	DecFskThrd(DECFskEnv *hEnv)
{
	UDWBuf_t			buf_t;
	int					i,szBuf;

	struct	timeval		start,end;
	printf("DecFskThrd...\n");

	szBuf= hEnv->hEnv->szBuf*sizeof(float);

	while(1){
		MBX_pend(hEnv->mbxReady,(char*)&buf_t,-1);

		CMEM_cacheWb(buf_t.pVirt,buf_t.actual);

		hEnv->pDecParams->pInData = buf_t.pPhys;
		hEnv->pDecParams->szInData= buf_t.actual;

		gettimeofday(&start,NULL);
		hEnv->hDecAlg->fxnProcess(hEnv->hDecAlg,0);
		gettimeofday(&end,NULL);

		printf("Dec consume time:%d\n",(end.tv_usec - start.tv_usec)/1000 + (end.tv_sec - start.tv_sec)*1000);

		MBX_post(hEnv->mbxFree,(char*)&buf_t,-1);
	}
}

static	DECFskEnv*	DECFskDemo_init(UDWEnv *hEnv)
{
	DECFskEnv		*decEnv;
	pthread_t		id;

	int				i;
	UDWBuf_t		buf_t;
	CROSSBuf		outBuf;

	decEnv = (char*)malloc(sizeof(DECFskEnv));
	decEnv->hEnv       = hEnv;

	decEnv->hSoutAlg   = SOUT_create();
	decEnv->pSoutParam = (PSOUTParam)((PMSGObj)decEnv->hSoutAlg->param.virt)->msgBuf;
	decEnv->pSoutParam->cbArg = decEnv;
	decEnv->pSoutParam->fxnProcCB = fxnSoutCB;

	decEnv->hDecAlg = DECFSK_create();
	decEnv->pDecParams = (PDECFSKParam)((PMSGObj)decEnv->hDecAlg->param.virt)->msgBuf;

	decEnv->pDecParams->nSplite   = 240*16; //short
	decEnv->pDecParams->szOutData = 45;
	decEnv->pDecParams->nDataArray= 10;


	outBuf.virt = (char*)CMEM_alloc(decEnv->pDecParams->szOutData*decEnv->pDecParams->nDataArray,&attrs);
	outBuf.phys = CMEM_getPhys(outBuf.virt);
	decEnv->pCrossBuf = (PCROSSBuf)CMEM_alloc(sizeof(CROSSBuf)*decEnv->pDecParams->nDataArray,&attrs);
	for(i = 0; i < decEnv->pDecParams->nDataArray; i ++){
		decEnv->pCrossBuf[i].virt = outBuf.virt + i * decEnv->pDecParams->szOutData;
		decEnv->pCrossBuf[i].phys = outBuf.phys + i * decEnv->pDecParams->szOutData;
		printf("[%d]:%08X->%08x\n",i,decEnv->pCrossBuf[i].virt,decEnv->pCrossBuf[i].phys);
	}
	CMEM_cacheWb(decEnv->pCrossBuf,sizeof(CROSSBuf)*decEnv->pDecParams->nDataArray);

	decEnv->pDecParams->outDataArray = (PCROSSBuf)CMEM_getPhys(decEnv->pCrossBuf);

	sprintf(decEnv->pDecParams->base.name,"DECFSK");
	CODECS_rpeDebug(decEnv->hDecAlg,TRUE);

	decEnv->hDecAlg->fxnCreate(decEnv->hDecAlg);
	CODECS_nodeLink(decEnv->hDecAlg,decEnv->hSoutAlg,ALG_FUNC_PROCESS);
	printf("Create OK:%d.\n",hEnv->nCapBuf);

	decEnv->mbxFree  = MBX_create(hEnv->nCapBuf+2,sizeof(UDWBuf_t));
	decEnv->mbxReady = MBX_create(hEnv->nCapBuf+2,sizeof(UDWBuf_t));

	for(i = 0; i < hEnv->nCapBuf; i ++){

		buf_t.actual = hEnv->szBuf;
		buf_t.size   = hEnv->szBuf;
		buf_t.pVirt  = (char*)CMEM_alloc(hEnv->szBuf,&attrs);
		buf_t.pPhys  = (char*)CMEM_getPhys(buf_t.pVirt);

		printf("Create Buf:%d->%08x:%08x\n",i,buf_t.pVirt,buf_t.pPhys);
		MBX_post(decEnv->mbxFree,(char*)&buf_t,-1);
	}

	//pthread_create(&id,NULL,ADThrd,decEnv);
	pthread_create(&id,NULL,DecFskThrd,decEnv);

	return	decEnv;
}

void	DecFskDemo(UDWEnv *hEnv,int argc,char* argv[])
{
	DECFskEnv   *decEnv;
	UDWBuf_t	buf_t;
	int			size;
	int			hr = 0;
	FILE		*fp = fopen(argv[2],"rb");
	decEnv = DECFskDemo_init(hEnv);

	char		*rbuf;
	int			szRbuf = hEnv->szBuf*2;
	rbuf = (short*)malloc(szRbuf);

	while(1){
		printf("Read size:");
		scanf("%d",&size);
		MBX_pend(decEnv->mbxFree,(char*)&buf_t,-1);
	//	size = size > buf_t.size ? buf_t.size : size;

		hr = fread(rbuf,szRbuf,1,fp);
		if(hr <= 0){
			fseek(fp,0,SEEK_SET);
			hr = fread(rbuf,szRbuf,1,fp);
		}

		{
			short	*pInBuf = (short*)rbuf;
			short	*pDst   = (short*)buf_t.pVirt;
			int		i;
			for(i = 0; i < hEnv->szBuf/2; i +=2){
				*pDst ++ = *pInBuf++;
				pInBuf++;
			}
			buf_t.actual = hEnv->szBuf;
		}
		MBX_post(decEnv->mbxReady,(char*)&buf_t,-1);
	}

}
