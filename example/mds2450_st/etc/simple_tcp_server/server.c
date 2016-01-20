/*******************************
 * Filename: server.c
 * Title: Simple TCP Server
 * Desc: Simple TCP Server
 *******************************/
 #include "inet.h"
 
 int main ( int argc, char* argv[] ) {
    int sockfd, newsockfd, clilen;
    struct sockaddr_in  cli_addr;
	struct sockaddr_in  serv_addr;
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
    newsockfd = accept( sockfd, (struct sockaddr *) &cli_addr, &clilen );
    
    if ( newsockfd < 0 ) {
        puts("Server: accept error!");
        exit(1);
    }
    
    if ( read(newsockfd, buff, 20) <= 0 ) {
        puts( "Server: readn error!");
        exit(1);
    }
    printf("Server: Received String = %s \n", buff);
    close( sockfd );
    close( newsockfd );

    return 0;
}
/****************************************
 Run:
*****************************************
[root@2440REBIS host]$ ./server.exe
Server: Received String = Hello world!~
****************************************/
