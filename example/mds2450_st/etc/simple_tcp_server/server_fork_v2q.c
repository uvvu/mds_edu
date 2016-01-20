/*******************************
 * Filename: server_fork_v2.c
 * Title: Simple TCP Server
 * Desc: ���ü� - ���μ������� 
 *******************************/
 #include "inet.h"
 #include <unistd.h>
 #include <sys/types.h>
 #include <sys/wait.h>
 
 int main ( int argc, char* argv[] ) {
    int sockfd, newsockfd, clilen;
    struct sockaddr_in  cli_addr;
	struct sockaddr_in  serv_addr;
	
	int pid;
    
	char buff[30];
    pname = argv[0];
    
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
        puts( "Server: ��Ʈ�� ������ �� �� �����ϴ�.");
        exit(1);
    }
    
    /* ������ �ּҸ� ����Ͽ� Ŭ���̾�Ʈ�� ���� �����ϰ� �Ѵ�. */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl( INADDR_ANY );
    serv_addr.sin_port = htons( SERV_TCP_PORT );
    
    if ( bind( sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0 ) {
        puts( "Server: ������巹���� ���ε� �� �� �����ϴ�.");
        exit(1);
    }
    
    listen( sockfd, 5 );
    clilen = sizeof( cli_addr );
    
    /* �ڽ��� ����ó��� �����Ѵ� */
	signal(SIGCHLD, SIG_IGN);
    
    while(1) {
    
        newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, &clilen );
        if ( newsockfd < 0 ) {
            puts("Server: accept error!"); exit(1);
        }
        
        /* TODO: fork�� ����Ͽ� ���μ��� �����Ѵ�.  pid�� ��� */
        
        if (pid < 0 ) {  // fork error
            /* TODO: ���μ����� ������ �� ������ ���� ó�� */
        }
            
        if ( pid==0 ) {  // child process
            /* TODO: �ڽ� ���μ����� ���� ó��, read()���� ����Ѵ�. */
        }
        close( newsockfd );
	}
    return 0;
}
