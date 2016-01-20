/* ********************************************
 * key_test.c 
 * Desc: Syncronized the user process.
 * - sleep and wakeup process using wait queue
 * ********************************************/
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define GET_TIMEOFDAY 		1
#define GET_EXECUTIONTIME 	2
#define IOCTL_READ_WRITE	_IOWR(0xA1, 0x01, int)

int main( int argc, char **argv )
{
    int fd;
    int err;
    int msgtype = 0;
	int msg = 0;
    int i;

	if( argc<2 ) {
		printf("USAGE: key_test [msgtype 0, 1 or 2]\n");
		exit(1);
	}

    msgtype = atoi(argv[1]);

    fd = open("/dev/hwkey", O_RDWR);
	if (fd < 0) {
        fd = open("/dev/misc/hwkey", O_RDWR);
        if (fd < 0) {
            printf("open hwkey driver failed\n");
            return -1;
        }
    }
    
	switch (msgtype) {
		case GET_TIMEOFDAY:
			if( (err = ioctl(fd, IOCTL_READ_WRITE, &msgtype) < 0 )) {
				printf("ioctl() error\n");
			}
			break;
		case GET_EXECUTIONTIME:
			if( (err = ioctl(fd, IOCTL_READ_WRITE, &msgtype) < 0 )) {
				printf("ioctl() error\n");
			}
			break;
		default:
			break;
	}
	if (msgtype == 0) {
		for(i=0; i<10; i++) {
			msg = 0;
			// POINT: blocking mode, just wait...
			// TODO: read() 시스템콜을 사용해 msg 읽기 
			
			printf("read msg = %d\n", msg);
			fflush(stdout);
		}
	}
    close(fd);

    return 0;
}


