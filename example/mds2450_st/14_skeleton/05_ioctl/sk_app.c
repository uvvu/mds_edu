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

int main(void)
{
    int retn;
    int fd;

    int flag = 0;
    
    fd = open("/dev/SK", O_RDWR);
    printf("fd = %d\n", fd);
    
    if (fd<0) {
        perror("/dev/SK error");
        exit(-1);
    }
    else
        printf("SK has been detected...\n");
    
    getchar();
 
     /* POINT: fd�� ����Ű�� �ִ� ���Ͽ� 0, flag��� �ּҸ� �����Ѵ�.*/
    ioctl(fd, 0, flag); getchar();
    ioctl(fd, 1, flag); getchar();
    ioctl(fd, 2, flag); getchar();
    ioctl(fd, 3, flag); getchar();
    ioctl(fd, 4, flag); getchar();
    ioctl(fd, 5, flag); getchar();
    ioctl(fd, 6, flag); getchar();
    ioctl(fd, 7, flag); getchar();
    ioctl(fd, 8, flag); getchar();
    

    close(fd);
    
    return 0;
}
