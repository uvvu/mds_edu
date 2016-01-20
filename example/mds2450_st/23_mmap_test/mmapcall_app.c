/****************************************
 *	Filename: mmapcall_app.c
 *	Title: MMAP Test Application
 *	Desc:
 ****************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

#define	DEVICE_FILENAME	"/dev/mmapcall"

#define	MMAP_SIZE		0x1000			/* 4096 byte */

int main(void)
{
	int		dev;
	int		loop;
	char	*ptrdata;

	dev = open(DEVICE_FILENAME, O_RDWR|O_NDELAY);

	if(dev >= 0)
	{
		/* TODO: 메모리 영역 맵핑 시도 */


		/* 메모리 영역 해제 */
		munmap(ptrdata, MMAP_SIZE);

		close(dev);
	}

	return 0;
}
