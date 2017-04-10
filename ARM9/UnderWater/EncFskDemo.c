/**********************************************************
 * UnderWater.c
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

	FILE			*fp;
}ENCFskEnv;


static	CMEM_AllocParams	attrs = {CMEM_HEAP,CMEM_CACHED,128};

static	int	fxnSoutCB(PBASParam param,void* cbArg,void* content)
{
	ENCFskEnv	*encEnv = (ENCFskEnv*)cbArg;
	PCROSSBuf	pCrossBuf;
	int			idx = (int)content;
	UDWDABuf	wbuf;
	pCrossBuf = &encEnv->pCrossBuf[idx];
	printf("Call Back:%08x->%08X..\n",pCrossBuf->phys,pCrossBuf->virt);


	wbuf.buf = (char*)pCrossBuf->virt;
	wbuf.size= encEnv->pEncParams->szOutData;
	CMEM_cacheInv(wbuf.buf,wbuf.size);
	fwrite(wbuf.buf,wbuf.size,1,encEnv->fp);
}
static	void*	ENCFskDemo_init(UDWEnv *hEnv,char* fname)
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
		printf("[%d]:%08X->%08x\n",i,encEnv->pCrossBuf[i].virt,encEnv->pCrossBuf[i].phys);
	}
	CMEM_cacheWb(encEnv->pCrossBuf,sizeof(CROSSBuf)*encEnv->pEncParams->nDataArray);

	encEnv->pEncParams->outDataArray = (PCROSSBuf)CMEM_getPhys(encEnv->pCrossBuf);

	sprintf(encEnv->pEncParams->base.name,"ENCFSK");
	CODECS_rpeDebug(encEnv->hEncAlg,FALSE);

	encEnv->hEncAlg->fxnCreate(encEnv->hEncAlg);
	CODECS_nodeLink(encEnv->hEncAlg,encEnv->hSoutAlg,ALG_FUNC_PROCESS);
	printf("Create OK:%d.\n",hEnv->nCapBuf);

	//Open DA
	//encEnv->fd = open("/dev/UdwDA",O_RDWR);
	encEnv->fp = fopen(fname,"wb");
	return	encEnv;
}

static	void	ENCFskDemo_test(void *h)
{
	ENCFskEnv		*encEnv = (ENCFskEnv*)h;
	char			*pBuf;
	int				szBuf;

	int				size;
	int				val;

	struct	timeval		start,end;

	szBuf = 1024;
	pBuf  = (char*)CMEM_alloc(szBuf,&attrs);
	while(1){
		//
		printf("Input Size:");
		scanf("%d",&size);
		printf("Input Value:");
		scanf("%d",&val);

		if(size > szBuf){
			CMEM_free(pBuf,&attrs);
			szBuf = size + 1024;
			pBuf  = (char*)CMEM_alloc(szBuf,&attrs);
		}
		memset(pBuf,val,size);
		CMEM_cacheWb(pBuf,size);
		encEnv->pEncParams->pInData = (char*)CMEM_getPhys(pBuf);
		encEnv->pEncParams->szInData= size;

		gettimeofday(&start,NULL);
		encEnv->hEncAlg->fxnProcess(encEnv->hEncAlg,0);
		gettimeofday(&end,NULL);

		printf("Enc consume time:%d\n",(end.tv_usec - start.tv_usec)/1000 + (end.tv_sec - start.tv_sec)*1000);
	}
}

void	EncFskDemo(UDWEnv* hEnv,int argc,char* argv[])
{
	void	*h;
	h = ENCFskDemo_init(hEnv,argv[2]);
	ENCFskDemo_test(h);
}
