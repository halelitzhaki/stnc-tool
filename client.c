#include "client.h"
void clientStart(int argc, char **argv) {
    int sock;
    struct sockaddr_in serverAddr;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock == 0) {
        printf("failed openning socket\n");
        exit(1);
    }

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = atoi(argv[3]);
    serverAddr.sin_addr.s_addr = inet_addr(argv[2]);

    if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) != 0) {
        printHelpClient();
        exit(1);
    }

    printf("connected to server successfully\n");

    if (argc > 4 && strcmp(argv[4], P) == 0) { // performance tool
        char msg[1024];
        strcpy(msg, argv[6]);
        strcat(msg, ",");
        strcat(msg, argv[5]);

        if (write(sock, msg, strlen(msg)) == -1) {
            printf("sending failed\n");
            exit(1);
        }
        performanceTool(sock, argv);
    }
    else { // chat tool
        fd_set read_fds;
        int max_fd;
        while(1) {
            char buffer[1024];
            FD_ZERO(&read_fds);
            FD_SET(STDIN_FILENO, &read_fds);
            FD_SET(sock, &read_fds);
            max_fd = (sock > STDIN_FILENO) ? sock : STDIN_FILENO;

            if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
                perror("select");
                exit(1);
            }
            if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            fgets(buffer, 1024, stdin);
                if (send(sock, buffer, strlen(buffer), 0) < 0) {
                    perror("send");
                    exit(1);
                }
            }
            if (FD_ISSET(sock, &read_fds)) {
                recv(sock, buffer, 1024 - 1, 0);
                printf("%s", buffer);
            }
        }
        close(sock);
    }
}

void performanceTool(int sock, char** argv) {
    if(strcmp(argv[5], IPV4) == 0 && strcmp(argv[6], TCP) == 0) ipv4Tcp(sock);
    else if(strcmp(argv[5], IPV6) == 0 && strcmp(argv[6], TCP) == 0) ipv6Tcp(sock, argv);
    else if(strcmp(argv[5], IPV4) == 0 && strcmp(argv[6], UDP) == 0) ipv4Udp(sock, argv);
    else if(strcmp(argv[5], IPV6) == 0 && strcmp(argv[6], UDP) == 0) ipv6Udp(sock, argv);
    else if(strcmp(argv[5], UDS) == 0 && strcmp(argv[6], STREAM) == 0) udsStream(sock);
    else if(strcmp(argv[5], UDS) == 0 && strcmp(argv[6], DGRAM) == 0) udsDgram(sock);
    else if(strcmp(argv[5], PIPE) == 0) clientPipe(sock, argv[6]);
    else if(strcmp(argv[5], MMAP) == 0) clientMmap(sock, argv[6]);
    else {
        printHelpClient();
        close(sock);
    }
}

char *createChunk() {
    int length = (100 * 1024 * 1024);
    char *chunk = (char *)malloc(length);
    for (int i = 0; i < length; i++) chunk[i] = 'x';
    return chunk;
}

unsigned short calculate_checksum(unsigned short *ptr, int nbytes) {
    unsigned long sum = 0;
    while (nbytes > 1) {
        sum += *ptr++;
        nbytes -= 2;
    }
    if (nbytes == 1) {
        sum += *(unsigned char*)ptr;
    }
    sum = (sum >> 16) + (sum & 0xffff);
    sum += (sum >> 16);
    return (unsigned short)(~sum);
}

void ipv4Tcp(int sock) {
    char *chunk = createChunk();
    int sentData = 0, remainedData = strlen(chunk), length = remainedData, packetSize = TCP_SEG_MAX;
    do {
        if (write(sock, chunk + sentData, packetSize) == -1) printf("error");
        sentData += packetSize;
        remainedData -= packetSize;
        packetSize = remainedData > TCP_SEG_MAX ? TCP_SEG_MAX : remainedData;
    } while (sentData < length);
    free(chunk);
    close(sock);
}


void ipv6Tcp(int sock, char**argv) {
    close(sock);
    char *chunk = createChunk();
    struct sockaddr_in6 serverAddr;
    int sentData = 0, remainedData = strlen(chunk), length = remainedData, packetSize = TCP_SEG_MAX;

    bzero(&serverAddr, sizeof(serverAddr));
    sock = socket(AF_INET6, SOCK_STREAM, 0);
    if(sock == 0) {
        printf("\nfailed openning ipv6 socket\n");
        exit(1);
    }

    serverAddr.sin6_family = AF_INET6;
    serverAddr.sin6_port = htons(atoi(argv[3]));

    if (inet_pton(AF_INET6, "::1", &serverAddr.sin6_addr) < 0) {
        printf("\nlocalhost not supported.\n");
        exit(1);
    }

    if (connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("\nfailed connecting tcp ipv6 server\n");
        exit(1);
    }


    do {
        if (send(sock, chunk + sentData, packetSize, 0) < 0) printf("error");
        sentData += packetSize;
        remainedData -= packetSize;
        packetSize = remainedData > TCP_SEG_MAX ? TCP_SEG_MAX : remainedData;
    } while (sentData < length);
    free(chunk);
    close(sock);
}

void ipv4Udp(int sock, char**argv) {
    close(sock);
    char *chunk = createChunk(), buffer[UDP_SEG_MAX];
    struct sockaddr_in serverAddr;
    int sentData = 0, remainedData = strlen(chunk), length = remainedData, packetSize = UDP_SEG_MAX;
      
    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(atoi(argv[3]));
    serverAddr.sin_family = AF_INET;
      
    sock = socket(AF_INET, SOCK_DGRAM, 0);
      
    if(connect(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("\nfailed connecting udp ipv4 server\n");
        exit(1);
    }
  
    do {
        unsigned short checksum = calculate_checksum((unsigned short*)(chunk + sentData), UDP_SEG_MAX - 2);
        memcpy(buffer, chunk + sentData, UDP_SEG_MAX - 2);
        memcpy(buffer + (UDP_SEG_MAX - 2), &checksum, sizeof(unsigned short));
        sendto(sock, buffer, packetSize, 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        sentData += packetSize;
        remainedData -= packetSize;
        packetSize = remainedData > UDP_SEG_MAX ? UDP_SEG_MAX : remainedData;
    } while (sentData < length);
    free(chunk);
    close(sock);
}

void ipv6Udp(int sock, char** argv) {
    close(sock);

    char *chunk = createChunk(), buffer[UDP_SEG_MAX];
    struct sockaddr_in6 serverAddr, clientAddr;
    int sentData = 0, remainedData = strlen(chunk), length = remainedData, packetSize = UDP_SEG_MAX, opt = 1;
      
    int udpSock = socket(AF_INET6, SOCK_DGRAM, 0);
    if(udpSock == 0) {
        printf("\nfailed openning ipv6 socket\n");
        exit(1);
    }

    bzero(&serverAddr, sizeof(serverAddr));
    serverAddr.sin6_port = htons(atoi(argv[3]));
    serverAddr.sin6_family = AF_INET6;

    if (inet_pton(AF_INET6, "[::1]", &serverAddr.sin6_addr) < 0) {
        printf("\nlocalhost not supported.\n");
        exit(1);
    }

    if(connect(udpSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        printf("\nfailed connecting udp ipv6 server\n");
        exit(1);
    }
  
    do {
        unsigned short checksum = calculate_checksum((unsigned short*)(chunk + sentData), UDP_SEG_MAX - 2);
        memcpy(buffer, chunk + sentData, UDP_SEG_MAX - 2);
        memcpy(buffer + (UDP_SEG_MAX - 2), &checksum, sizeof(unsigned short));
        sendto(udpSock, chunk + sentData, packetSize, 0, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
        sentData += packetSize;
        remainedData -= packetSize;
        packetSize = remainedData > UDP_SEG_MAX ? UDP_SEG_MAX : remainedData;
    } while (sentData < length);
    free(chunk);
    close(udpSock);
}

void udsDgram(int sock) {
    close(sock);

    char *chunk = createChunk();
    struct sockaddr_un addr;
    int sentData = 0, remainedData = strlen(chunk), length = remainedData, packetSize = UDS_MAX, opt = 1;

    sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock == -1) {
        printf("\nfailed openning unix dgram socket\n");
        exit(1);
    }

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, DGRAM_SOCKET_PATH, sizeof(addr.sun_path)-1);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        printf("\nfailed connecting unix sgram server\n");
        exit(1);
    }

    do {
        sendto(sock, chunk + sentData, packetSize, 0, (struct sockaddr*)&addr, sizeof(addr));
        sentData += packetSize;
        remainedData -= packetSize;
        packetSize = remainedData > UDS_MAX ? UDS_MAX : remainedData;
    } while (sentData < length);
    free(chunk);
    close(sock);
}

void udsStream(int sock) {
    close(sock);

    struct sockaddr_un serverAddr;
    char *chunk = createChunk();
    int sentData = 0, remainedData = strlen(chunk), length = remainedData, packetSize = UDS_MAX;

    sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock == -1) {
        printf("\nfailed openning unix stream socket\n");
        exit(1);
    }

    serverAddr.sun_family = AF_UNIX;
    strcpy(serverAddr.sun_path, STREAM_SOCKET_PATH);

    if (connect(sock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        printf("\nfailed connecting unix stream server\n");
        exit(1);
    }

    do {
        if (write(sock, chunk + sentData, packetSize) == -1) printf("error");
        sentData += packetSize;
        remainedData -= packetSize;
        packetSize = remainedData > UDS_MAX ? UDS_MAX : remainedData;
    } while (sentData < length);
    free(chunk);
    close(sock);
}

void clientPipe(int sock, char * fifo) {
    close(sock);

    char *chunk = createChunk();
    char s[PIPE_MAX];
    int num = 0, fd, sentData = 0, remainedData = strlen(chunk), length = remainedData, packetSize = PIPE_MAX;

    mknod(fifo, S_IFIFO | 0666, 0);

    fd = open(fifo, O_WRONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    do {
        if (write(fd, chunk + sentData, packetSize) == -1) printf("error");
        sentData += packetSize;
        remainedData -= packetSize;
        packetSize = remainedData > PIPE_MAX ? PIPE_MAX : remainedData;
    } while (sentData < length);
    free(chunk);
    char * argv[10] = {"rm", fifo, NULL};
    execvp(argv[0], argv);
}

void clientMmap(int sock, char * file) {
    close(sock);
    char *chunk = createChunk(), *data;
    int fd, sentData = 0, remainedData = strlen(chunk), packetSize = UDS_MAX;

    fd = shm_open(file, O_RDWR | O_CREAT, 0644);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }
    if (ftruncate(fd, UDS_MAX) == -1) {
        perror("ftruncate");
        exit(EXIT_FAILURE);
    }
    data = mmap(NULL, UDS_MAX, PROT_WRITE, MAP_SHARED, fd, 0);
    if(data == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    do {
        snprintf(data, packetSize, "%s", chunk + sentData);
        data = mmap(NULL, UDS_MAX, PROT_WRITE, MAP_SHARED, fd, 0);
        if(*((char*)data) != '\0') break;
        sentData += packetSize;
        remainedData -= packetSize;
        packetSize = remainedData > UDS_MAX ? UDS_MAX : remainedData;
    } while (sentData < 100 * 1024 * 1024);

    munmap(data, UDS_MAX);
    free(chunk);
    close(fd);
}

void printHelpClient() {
    printf("stnc:\n\nUsage: ./stnc [client/server] <parameters>\n\nRUN SERVER BEFORE CLIENT!\n\nFor server:\n ./stnc -s <PORT> <flag> <flag>\noptions:\n    -p      performance test\n    -q      quiet mode\n\n\nFor client:\n ./stnc -c <IP> <PORT> <flag> <type> <param>\nflag options:\n    -p      performance test\n\ntype options:\n     ipv4\n     ipv6\n     uds\n     mmap\n     pipe\n\nparam options:\n     tcp\n     udp\n     dgram\n     stream\n     filename\n");
}