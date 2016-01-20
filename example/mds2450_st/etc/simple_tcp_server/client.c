/*******************************
 * Filename: client.c
 * Title: Simple TCP Server
 * Desc: Simple TCP Server
 *******************************/
 #include "inet.h"
 
 int main ( int argc, char *argv[]) {
    int sockfd;
    struct sockaddr_in serv_addr;
    pname = argv[0];
    
    /* �����ϰ��� �ϴ� ������ �ּ� �� ��Ʈ��ȣ�� ����ü serv_addr�� ä��� */
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr( SERV_HOST_ADDR );
    serv_addr.sin_port = htons( SERV_TCP_PORT );
    
    /* TCP Socket Open */
    if ((sockfd = socket( AF_INET, SOCK_STREAM, 0 )) < 0 ) {
        puts( "Client: ������ ���� �����ϴ�.");
        exit(1);
    }
    
    /* ������ ���� */
    if ( connect( sockfd, (struct sockaddr *) &serv_addr, sizeof( serv_addr )) < 0 ) {
        puts( "Client: ������ ������ �� �����ϴ�.");
        exit(1);
    }
    
    if ( write( sockfd, "Hello world!~", 20) < 20 ) {
        puts( "Client: writen error" );
        exit(1);
    }
    
    close( sockfd );
}
