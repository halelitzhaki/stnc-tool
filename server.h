#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <netinet/udp.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <sys/time.h>
#include <poll.h>
#include <errno.h>

#define IPV4 "ipv4"
#define IPV6 "ipv6"
#define TCP "tcp"
#define UDP "udp"
#define UDS "uds"
#define PIPE "pipe"
#define MMAP "mmap"
#define DGRAM "dgram"
#define STREAM "stream"
#define P "-p"
#define Q "-q"
#define TCP_SEG_MAX 65535
#define UDP_SEG_MAX 65507
#define STREAM_SOCKET_PATH "/tmp/stream_socket"
#define DGRAM_SOCKET_PATH "/tmp/dgram_socket"
#define UDS_MAX 4096
#define PIPE_MAX 300

void serverStart(int, char **);
void serverPerformanceTool(int, char**, int);
char* getFileFromBuf(char*);
unsigned short calculateChecksum(unsigned short *, int);
double serverIpv4Tcp(int, int);
double serverIpv6Tcp(int, int, int);
double serverIpv4Udp(int, int, int);
double serverIpv6Udp(int, int, int);
double serverUdsDgram(int, int);
double serverUdsStream(int, int);
double serverMmap(int, char*, int);
double serverPipe(int, char*, int);
void printHelp();


#endif