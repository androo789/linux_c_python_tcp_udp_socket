/**********************************************************
 * _ALGItf.h
 *
 * 
 *********************************************************/

#ifndef ALGITF_H_
#define ALGITF_H_

#include	<BurchFw.h>
#include	<mbx.h>
#if defined (__cplusplus)
extern "C" {
#endif /*defined (__cplusplus)*/

typedef	struct{
	PALGIntf		hAlg;
	PBASParam		params;
	void*			rsv;
}ITFNode,*PITFNode;


typedef	struct{
	char	*pVirt;
	char	*pPhys;
	int		size;
	int		actual;
}UDWBuf_t;

typedef	struct{
	int		nCapBuf;
	int		szBuf;

	float		enc_scaler;
	int			enc_szPkt;
	int			enc_szOutData;

	int			fdGain;

	FILE		*fp;
	int			bSave;

	int			bSending;
	MBX_Handle	mbxSendWait;
}UDWEnv;


void	DECFsk_init(UDWEnv *hEnv);
#if defined (__cplusplus)
}
#endif /*defined (__cplusplus)*/
#endif /* ALGITF_H_ */
