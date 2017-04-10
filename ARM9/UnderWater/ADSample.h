/**********************************************************
 * ADSample.h
 *
 * 
 *********************************************************/

#ifndef ADSAMPLE_H_
#define ADSAMPLE_H_

#if defined (__cplusplus)
extern "C" {
#endif /*defined (__cplusplus)*/

typedef	struct{
	char	*pVirt;
	char	*pPhys;
	int		size;
	int		actual;
}BufInfo_t;

typedef	struct{
	BufInfo_t	*bufInfo;
	char		*pDst;
	int			size;
}CPYBuf_t;
#define	MAGIC_NUM	'g'

#define	AD_CMD_LOCK		_IO(MAGIC_NUM,4)
#define	AD_CMD_UNLOCK	_IO(MAGIC_NUM,5)
#define	AD_CMD_SET		_IO(MAGIC_NUM,6)
#define	AD_CMD_COPY		_IO(MAGIC_NUM,7)

#if defined (__cplusplus)
}
#endif /*defined (__cplusplus)*/
#endif /* ADSAMPLE_H_ */
