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
    
    /* TODO: sockfd�� AF_INET, SOCK_STREAM,0���� ���� ����
    
    /* ������ �ּҸ� ����Ͽ� Ŭ���̾�Ʈ�� ���� �����ϰ� �Ѵ�. */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl( INADDR_ANY );
    serv_addr.sin_port = htons( SERV_TCP_PORT );
    
    /* TODO: �ּҸ� ���ε� �Ѵ�. (struct sockaddr *) &serv_addr */
    
    listen( sockfd, 5 );
    clilen = sizeof( cli_addr );
    
    /* TODO: accept()�Լ��� �̿��Ͽ� �ּҸ� �����Ѵ�. newsockfd */
    newsockfd = /* TODO */
    
    /* TODO: read()�Լ��� ��Ŷ�� �о� �´�. buff�� ���� 20��ŭ ���� �Ѵ�. */

    printf("Server: Received String = %s \n", buff);
    close( sockfd );
    close( newsockfd );

    return 0;
}
/****************************************
 Run:
*****************************************
$ ./server.exe
Server: Received String = Hello world!~
****************************************/
