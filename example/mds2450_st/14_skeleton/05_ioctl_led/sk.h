#ifndef _SK_H_
#define	_SK_H_

#define	SK_MAGIC	'k'
#define	SK_MAXNR	6

typedef struct {
	unsigned long size;
	unsigned char buff[128];
} __attribute__((packed)) sk_info;

#define	SK_LED_OFF		_IO(SK_MAGIC, 0)
#define	SK_LED_ON		_IO(SK_MAGIC, 1)
#define	SK_GETSTATE		_IO(SK_MAGIC, 2)

#define	SK_READ			_IOR(SK_MAGIC, 3, sk_info)
#define	SK_WRITE		_IOW(SK_MAGIC, 4, sk_info)
#define	SK_RW			_IOWR(SK_MAGIC, 5, sk_info)

#endif	/* _SK_H_ */

