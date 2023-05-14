#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

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
#define TCP_SEG_MAX 65535
#define UDP_SEG_MAX 65507
#define STREAM_SOCKET_PATH "/tmp/stream_socket"
#define DGRAM_SOCKET_PATH "/tmp/dgram_socket"
#define UDS_MAX 4096
#define PIPE_MAX 300

void clientStart(int, char **);
void performanceTool(int, char**);
char *createChunk();
unsigned short calculate_checksum(unsigned short *, int);
void ipv4Tcp(int);
void ipv6Tcp(int, char**);
void ipv4Udp(int, char**);
void ipv6Udp(int, char**);
void udsDgram(int);
void udsStream(int);
void clientMmap(int, char*);
void clientPipe(int, char*);
void printHelpClient();

#endif