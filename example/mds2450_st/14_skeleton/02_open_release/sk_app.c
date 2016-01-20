/***************************************
 * Filename: sk_app.c
 * Title: Skeleton Device Application
 * Desc: Implementation of system call
 ***************************************/
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

int main(void)
{
    int fd;
    
    /* TODO use open systemcall with /dev/sk */
    fd = open("/dev/sk", O_RDWR);
    printf("fd = %d\n", fd);
    
    if (fd<0) {
        perror("/dev/sk error");
        exit(-1);
    }
    else
        printf("sk has been detected...\n");
    
    getchar();
    close(fd);
    
    return 0;
}
