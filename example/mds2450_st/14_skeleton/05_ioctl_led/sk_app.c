/***************************************
 * Filename: sk_app.c
 * Title: Skeleton Device Application
 * Desc: Implementation of system call
 ***************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <termio.h>

#include "sk.h"

int main(void)
{
    int retn;
    int fd;

    int flag = 0;
    
    fd = open("/dev/sk", O_RDWR);
    printf("fd = %d\n", fd);
    
    if (fd<0) {
        perror("/dev/sk error");
        exit(-1);
    }
    else
        printf("sk has been detected...\n");
    
    getchar();
 
     /* fd가 가르키고 있는 파일에 0, flag라는 주소를 전달한다.*/
    ioctl(fd, SK_LED_ON, flag); getchar();
    ioctl(fd, SK_LED_OFF, flag); getchar();
    ioctl(fd, SK_LED_ON, flag); getchar();
    ioctl(fd, SK_LED_OFF, flag); getchar();
    ioctl(fd, SK_LED_ON, flag); getchar();
    ioctl(fd, SK_LED_OFF, flag); getchar();
    ioctl(fd, SK_LED_ON, flag); getchar();
    ioctl(fd, SK_LED_OFF, flag); getchar();

    close(fd);
    
    return 0;
}
