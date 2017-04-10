/**********************************************************
 * u_boot_env.c
 *
 * 
 *********************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define	U_BOOT_ENV_FILE	"/dev/mtdblock0"
#define BOOT_ENV_SIZE	128*1024

char*	GetKeySect(char* buf,char* sect)
{
	char	*pStart;
	char	*pBuf = buf+4;
	do{
		if((pBuf[0] == 0) && (pBuf[1] == 0)) return NULL;
		pStart = strstr(pBuf,sect);
		if(pStart != pBuf) pStart = NULL;
		if(pStart==NULL){
			pBuf += strlen(pBuf) + 1;
		}
	}while(pStart== NULL);

	return	pStart;
}

static	int GetEndSec(char* buf)
{
	char	*pStart = buf;
	buf += 4;
	do{
		buf ++;
	}while((buf[0] != 0) || (buf[1] != 0x0));

	return (int)(buf - pStart);
}
void	UBOOTEnv_init(char* buf,int sz)
{
	int			fd;
	fd = open(U_BOOT_ENV_FILE,O_RDWR);
	if(fd <= 0) return;

	read(fd,buf,sz);

	close(fd);
}


void	UBOOTEnv_save(char* buf,int sz)
{
	int				fd;
	unsigned int	crc;

	//UpdateUBootArgs(buf);

	crc = uboot_crc32(0,buf+4,sz - sizeof(unsigned int));
	buf[0] = crc&0xff;
	buf[1] = (crc>>8)&0xff;
	buf[2] = (crc>>16)&0xff;
	buf[3] = (crc>>24)&0xff;

	system("flash_eraseall /dev/mtd0");

	fd = open(U_BOOT_ENV_FILE,O_RDWR);
	write(fd,buf,sz);
	close(fd);
}
void	UBOOTEnv_set(char* ip,char* gw,char* mask,char* mac)
{
	char	*pUbootBuf = (char*)malloc(BOOT_ENV_SIZE);
	char	*pBootArgs,*pBootArgsTmp,*pArgs;
	int		szArgs;

	UBOOTEnv_init(pUbootBuf,BOOT_ENV_SIZE);

	pArgs = GetKeySect(pUbootBuf,"bootargs=");

	szArgs = strlen(pArgs)+1024;
	pBootArgs = (char*)malloc(szArgs);
	memset(pBootArgs,0,szArgs);
	char* pIp = strstr(pArgs,"ip=");
	if(pIp){
		memcpy(pBootArgs,pArgs,(int)(pIp-pArgs));
		sprintf(pBootArgs+strlen(pBootArgs),"ip=%s:%s:%s",ip,gw,mask);

		pBootArgsTmp = strstr(pIp," ");
		if(pBootArgsTmp){
			sprintf(pBootArgs + strlen(pBootArgs),"%s",pBootArgsTmp);
		}
	}

	char* pEth = strstr(pBootArgs,"eth=");
	if(pEth){
		char	*ptmp;
		pBootArgsTmp = (char*)malloc(szArgs);
		memset(pBootArgsTmp,0,szArgs);
		memcpy(pBootArgsTmp,pBootArgs,(int)(pEth-pBootArgs));
		sprintf(pBootArgsTmp+strlen(pBootArgsTmp),"eth=%s",mac);
		ptmp = strstr(pEth," ");
		if(ptmp){
			sprintf(pBootArgsTmp + strlen(pBootArgsTmp),"%s",ptmp);
		}
		sprintf(pBootArgs,"%s",pBootArgsTmp);
		free(pBootArgsTmp);
	}

	char	*pBootBuf = (char*)malloc(BOOT_ENV_SIZE);
	char	*pTempBuf;

	memset(pBootBuf,0x0,BOOT_ENV_SIZE);
	memcpy(pBootBuf,pUbootBuf,(int)(pArgs-pUbootBuf));
	pTempBuf = pBootBuf + (int)(pArgs-pUbootBuf);
//	printf("%s\n",pBootArgs);
	memcpy(pTempBuf,pBootArgs,strlen(pBootArgs));
	pTempBuf += strlen(pBootArgs) +1;

	int nRemain;
	int	total = GetEndSec(pUbootBuf);
//	printf("%d,%d\n",total,total - (int)(pArgs+strlen(pArgs) - pUbootBuf));
	memcpy(pTempBuf,pArgs + strlen(pArgs) + 1,GetEndSec(pUbootBuf)-(pArgs+strlen(pArgs)+1 - pUbootBuf));

	unsigned int crc;
	crc = uboot_crc32(0,pBootBuf+4,BOOT_ENV_SIZE - sizeof(unsigned int));
	pBootBuf[0] = crc&0xff;
	pBootBuf[1] = (crc>>8)&0xff;
	pBootBuf[2] = (crc>>16)&0xff;
	pBootBuf[3] = (crc>>24)&0xff;

	{
		system("flash_eraseall /dev/mtd0");

		int fd = open(U_BOOT_ENV_FILE,O_RDWR);
		write(fd,pBootBuf,BOOT_ENV_SIZE);
		close(fd);
	}
	free(pBootArgs);


	free(pUbootBuf);
}
void	UBOOTEnv_get(char* ip,char* gw,char* mask,char* mac)
{
	char	*pUbootBuf = (char*)malloc(BOOT_ENV_SIZE);
	char	*pBootArgs;
	char	*pPtr,*pPtrEnd;
	char	pChr;
	UBOOTEnv_init(pUbootBuf,BOOT_ENV_SIZE);

	pBootArgs = GetKeySect(pUbootBuf,"bootargs=");
//	printf("\n%s\n",pBootArgs);

	pPtr = strstr(pBootArgs,"eth=");
	if(pPtr){
		pPtr += strlen("eth=");
		pPtrEnd = strstr(pPtr," ");
		if(pPtrEnd== NULL){
			sprintf(mac,"%s",pPtr);
		}else{
			pChr = *pPtrEnd;
			*pPtrEnd = 0;
			sprintf(mac,"%s",pPtr);
			*pPtrEnd = pChr;
		}
	}else{
		mac[0] = 0;
	}

	//ªÒ»°IP:GW:MASK
	pPtr = strstr(pBootArgs,"ip=");
	if(pPtr){
		pPtr += strlen("ip=");
		pPtrEnd = strstr(pPtr," ");
		if(pPtrEnd!= NULL){
			*pPtrEnd = 0;
		}
		//ip
		pPtrEnd = strstr(pPtr,":");
		*pPtrEnd++ = 0;
		sprintf(ip,"%s",pPtr);
		pPtr = pPtrEnd;
		//gw
		pPtrEnd = strstr(pPtr,":");
		*pPtrEnd++ = 0;
		sprintf(gw,"%s",pPtr);
		pPtr = pPtrEnd;
		//mask
		sprintf(mask,"%s",pPtr);
	}else{
		ip[0] = 0;
		gw[0] = 0;
		mask[0] = 0;
	}
	free(pUbootBuf);
}
