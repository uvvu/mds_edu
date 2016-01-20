/*******************************
 * Filename: inet.h
 * Title: Simple TCP Server
 * Desc: Simple TCP Server
 *******************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERV_UDP_PORT   7000 /* UDP Server port */
#define SERV_TCP_PORT   7000 /* TCP Server port */
//#define SERV_HOST_ADDR  "192.168.0.2" /* Server IP address */
#define SERV_HOST_ADDR  "192.168.50.36" /* Server IP address */

char *pname;

