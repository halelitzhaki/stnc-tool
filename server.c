#include "server.h"

void serverStart(int argc, char **argv) {
    int serverSock, clientSock;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrSize;

    int quiet = 0;
    if((argc > 3 && strcmp(argv[3], Q) == 0) || (argc > 4 && strcmp(argv[4], Q) == 0)) quiet = 1;

    serverSock = socket(AF_INET, SOCK_STREAM, 0);
    if(serverSock == 0) {
        if(quiet == 0) printf("\nfailed openning socket\n");
        exit(1);
    }
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = atoi(argv[2]);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    if (bind(serverSock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) != 0) {
        if(quiet == 0) printf("\nsocket bind failed\n");
        exit(1);
    }
    if (listen(serverSock, 2) != 0) {
        if(quiet == 0) printf("socket listen failed\n");
        exit(1);
    }
    if(quiet == 0) printf("waiting for client...\n");
    clientAddrSize = sizeof(clientAddr);
    clientSock = accept(serverSock, (struct sockaddr *)&clientAddr, &clientAddrSize);
    if(clientSock < 0) {
        if(quiet == 0) printf("'nconnecting client failed\n");
        exit(1);
    }

    if(quiet == 0) printf("\nclient connected successfully\n");

    if ((argc > 3 && strcmp(argv[3], P) == 0) || (argc > 4 && strcmp(argv[4], P) == 0)) serverPerformanceTool(clientSock, argv, quiet); // performance tool

    else { // chat tool
        fd_set read_fds;
        int max_fd;
        while(1) {
            char buffer[1024];
            FD_ZERO(&read_fds);
            FD_SET(STDIN_FILENO, &read_fds);
            FD_SET(clientSock, &read_fds);
            max_fd = (clientSock > STDIN_FILENO) ? clientSock : STDIN_FILENO;

            if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
                if(quiet == 0) perror("select");
                exit(1);
            }
            if (FD_ISSET(STDIN_FILENO, &read_fds)) {
            fgets(buffer, 1024, stdin);
                if (send(clientSock, buffer, strlen(buffer), 0) < 0) {
                    if(quiet == 0) perror("send");
                    exit(1);
                }
            }
            if (FD_ISSET(clientSock, &read_fds)) {
                recv(clientSock, buffer, 1024 - 1, 0);
                printf("%s", buffer);
            }
        }
    }
}


void serverPerformanceTool(int sock, char ** argv, int quiet) {
    char buffer[1024];
    if (read(sock, buffer, sizeof(buffer)) == -1) {
        if(quiet == 0) printf("\nerror while receiving\n");
        exit(1);
    }

    if (strstr(buffer, IPV4) != NULL && strstr(buffer, TCP) != NULL) printf("%s_%s,%f\n", IPV4, TCP, serverIpv4Tcp(sock, quiet));
    else if (strstr(buffer, IPV6) != NULL && strstr(buffer, TCP) != NULL) printf("%s_%s,%f\n", IPV6, TCP, serverIpv6Tcp(sock, atoi(argv[2]), quiet));
    else if (strstr(buffer, IPV4) != NULL && strstr(buffer, UDP) != NULL) printf("%s_%s,%f\n", IPV4, UDP, serverIpv4Udp(sock, atoi(argv[2]), quiet));
    else if (strstr(buffer, IPV6) != NULL && strstr(buffer, UDP) != NULL) printf("%s_%s,%f\n", IPV6, UDP, serverIpv6Udp(sock, atoi(argv[2]), quiet));
    else if (strstr(buffer, UDS) != NULL && strstr(buffer, STREAM) != NULL) printf("%s_%s,%f\n", UDS, STREAM, serverUdsStream(sock, quiet));
    else if (strstr(buffer, UDS) != NULL && strstr(buffer, DGRAM) != NULL) printf("%s_%s,%f\n", UDS, DGRAM, serverUdsDgram(sock, quiet));
    else if(strstr(buffer, PIPE) != NULL) {
        char * file = getFileFromBuf(buffer);
        printf("%s,%f\n", PIPE, serverPipe(sock, file, quiet));
        free(file);
    }
    else if(strstr(buffer, MMAP) != NULL) {
        char * file = getFileFromBuf(buffer);
        printf("%s,%f\n", MMAP, serverMmap(sock, file, quiet));
        free(file);
    }
    else {
        printHelp();
        close(sock);
    }
}

char* getFileFromBuf(char* buffer) {
    int i = 0;
    for(i = 0; i < strlen(buffer); i++) {
        if(buffer[i] == ',') break;
    }
    char *filename = (char*)malloc(i+1);
    strncpy(filename, buffer, i);
    filename[i] = '\0';
    return filename;    
}

unsigned short calculateChecksum(unsigned short *ptr, int nbytes) {
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

double serverIpv4Tcp(int sock, int quiet) {
    char buffer[TCP_SEG_MAX];
    struct timeval start, end;
    gettimeofday(&start, NULL);
    while (read(sock, buffer, TCP_SEG_MAX) > 0) {
        //..
    }
    gettimeofday(&end, NULL);
    close(sock);
    return ((double)((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec)) / 1000000;
}

double serverIpv6Tcp(int sock, int port, int quiet) {
    close(sock);
    struct sockaddr_in6 addr;
    int serverSock, clientSock, opt = 1, addrlen = sizeof(addr);
    struct timeval start, end;
    char buffer[TCP_SEG_MAX];

    serverSock = socket(AF_INET6, SOCK_STREAM, 0);
    if(serverSock == 0) {
        if(quiet == 0) printf("\nfailed openning ipv6 socket\n");
        exit(1);
    }

    setsockopt(serverSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    addr.sin6_family = AF_INET6;
    addr.sin6_addr = in6addr_loopback;
    addr.sin6_port = htons(port);

    if (bind(serverSock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        if(quiet == 0) printf("\nbind failed\n");
        exit(1);
    }

    if(listen(serverSock, 2) < 0) {
        if(quiet == 0) printf("\nlisten failed\n");
        exit(1);
    }

    clientSock = accept(serverSock, (struct sockaddr *)&addr, (socklen_t *)&addrlen);
    if (clientSock < 0) {
        if(quiet == 0) perror("\nconnecting client failed\n");
        exit(1);
    }

    gettimeofday(&start, NULL);

    while (recv(clientSock, buffer, TCP_SEG_MAX, 0) > 0) {        
        //..
    }
    gettimeofday(&end, NULL);
    close(sock);
    return ((double)((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec)) / 1000000;
}


double serverIpv4Udp(int sock, int port, int quiet) {
    close(sock);

    struct sockaddr_in serverAddr, clientAddr;
    fd_set readfds;
    struct timeval start, end, tv;
    char buffer[UDP_SEG_MAX];
    int udpSock, bytesReceived = 0;

    udpSock = socket(AF_INET, SOCK_DGRAM, 0);
    if (udpSock == 0) {
        if(quiet == 0) perror("\nfailed openning udp ipv4 socket\n");
        exit(1);
    }

    serverAddr.sin_family = AF_INET; 
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(port);

    if (bind(udpSock, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        if(quiet == 0) perror("bind failed");
        exit(1);
    }

    int addrlen = sizeof(clientAddr);
    gettimeofday(&start, NULL);
    while (1) { 
        
        int bytes = recvfrom(udpSock, buffer, UDP_SEG_MAX, 0, (struct sockaddr *)&clientAddr, &addrlen);
        unsigned short received_checksum;
        memcpy(&received_checksum, buffer + bytes - 2, 2);
        unsigned short calculated_checksum = calculateChecksum((unsigned short*)(buffer + 2), bytes - 2);
        if (received_checksum == calculated_checksum) {
            if(quiet == 0) printf("Checksum mismatch, discarding packet\n");
            continue;
        }
        if (bytes < 0) {
            if(quiet == 0) perror("recvfrom failed");
            exit(1);
        }
        bytesReceived += bytes;

        FD_ZERO(&readfds);
        FD_SET(udpSock, &readfds);
        tv.tv_usec = 5000;

        int ready = select(udpSock + 1, &readfds, NULL, NULL, &tv);
        if (ready == -1) {
            if(quiet == 0) perror("select");
            exit(1);
        } else if (ready == 0) break;
    }
    gettimeofday(&end, NULL);
    close(udpSock);
    return ((double)((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec)) / 1000000;
}

double serverIpv6Udp(int sock, int port, int quiet) {
    close(sock);

    struct sockaddr_in6 serverAddr, clientAddr;
    fd_set readfds;
    struct timeval start, end, tv;
    char buffer[UDP_SEG_MAX];
    int udpSock, bytesReceived = 0, opt = 1;

    udpSock = socket(AF_INET6, SOCK_DGRAM, 0);
    if (udpSock == 0) {
        if(quiet == 0) perror("\nfailed openning udp ipv4 socket\n");
        exit(1);
    }

    setsockopt(udpSock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    serverAddr.sin6_family = AF_INET6; 
    serverAddr.sin6_addr = in6addr_loopback;
    serverAddr.sin6_port = htons(port);

    if (bind(udpSock, (const struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        if(quiet == 0) perror("bind failed");
        exit(1);
    }

    int addrlen = sizeof(clientAddr);
    gettimeofday(&start, NULL);
    while (1) { 
        int bytes = recvfrom(udpSock, buffer, UDP_SEG_MAX, 0, (struct sockaddr *)&clientAddr, &addrlen);
        unsigned short received_checksum;
        memcpy(&received_checksum, buffer + bytes - 2, 2);
        unsigned short calculated_checksum = calculateChecksum((unsigned short*)(buffer + 2), bytes - 2);
        if (received_checksum == calculated_checksum) {
            if(quiet == 0) printf("Checksum mismatch, discarding packet\n");
            continue;
        }
        if (bytes < 0) {
            if(quiet == 0) perror("recvfrom failed");
            exit(1);
        }
        bytesReceived += bytes;

        FD_ZERO(&readfds);
        FD_SET(udpSock, &readfds);
        tv.tv_usec = 5000;

        int ready = select(udpSock + 1, &readfds, NULL, NULL, &tv);
        if (ready == -1) {
            if(quiet == 0) perror("select");
            exit(1);
        } else if (ready == 0) break;
    }
    gettimeofday(&end, NULL);
    close(udpSock);

    return ((double)((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec)) / 1000000;
}

double serverUdsDgram(int sock, int quiet) {
    close(sock);

    struct sockaddr_un addr;
    fd_set readfds;
    struct timeval start, end, tv;
    int bytesReceived = 0;
    char buffer[UDS_MAX];

    sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (sock == -1) {
        if(quiet == 0) printf("\nfailed openning unix dgram socket\n");
        exit(1);
    }
    unlink(DGRAM_SOCKET_PATH);
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, DGRAM_SOCKET_PATH, sizeof(addr.sun_path)-1);

    if (bind(sock, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        if(quiet == 0) printf("\nbind failed\n");
        exit(1);
    }

    gettimeofday(&start, NULL);
    while (1) { 
        int bytes = recv(sock, buffer, UDS_MAX, 0);
        if (bytes < 0) {
            if(quiet == 0) perror("recvfrom failed");
            exit(1);
        }
        bytesReceived += bytes;

        FD_ZERO(&readfds);
        FD_SET(sock, &readfds);
        tv.tv_usec = 5000;

        int ready = select(sock + 1, &readfds, NULL, NULL, &tv);
        if (ready == -1) {
            if(quiet == 0) perror("select");
            exit(1);
        } else if (ready == 0) break;
    }
    gettimeofday(&end, NULL);
    close(sock);

    return ((double)((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec)) / 1000000;

}

double serverUdsStream(int sock, int quiet) {
    close(sock);

    struct sockaddr_un serverAddr, clientAddr;
    socklen_t length = sizeof(clientAddr);
    int serverSock, clientSock;
    char buffer[UDS_MAX];
    struct timeval start, end;

    serverSock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (serverSock == -1) {
        if(quiet == 0) printf("\nfailed openning unix stream socket\n");
        exit(1);
    }
    serverAddr.sun_family = AF_UNIX;
    strcpy(serverAddr.sun_path, STREAM_SOCKET_PATH);
    unlink(STREAM_SOCKET_PATH);

    if (bind(serverSock, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        if(quiet == 0) printf("\nbind failed\n");
        exit(1);
    }

    if (listen(serverSock, 2) == -1) {
        if(quiet == 0) printf("\nlisten failed\n");
        exit(1);
    }

    clientSock = accept(serverSock, (struct sockaddr*)&clientAddr, &length);
    if (clientSock == -1) {
        if(quiet == 0) printf("\nfailed connecting client\n");
        exit(1);
    }

    gettimeofday(&start, NULL);
    while (read(clientSock, buffer, UDS_MAX) > 0) {
        //..
    }
    gettimeofday(&end, NULL);

    close(clientSock);
    close(serverSock);

    return ((double)((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec)) / 1000000;    
}

double serverPipe(int sock, char * fifo, int quiet) {
    close(sock);

    struct timeval start, end;
    char buffer[PIPE_MAX];
    int num = 0, fd;

    mknod(fifo, S_IFIFO | 0666, 0);
    fd = open(fifo, O_RDONLY);
    if (fd == -1) {
        if(quiet == 0) perror("open");
        exit(EXIT_FAILURE);
    }

    gettimeofday(&start, NULL);
    do {
        num = read(fd, buffer, PIPE_MAX);
    } while (num > 0);
    gettimeofday(&end, NULL);

    return ((double)((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec)) / 1000000;
}

double serverMmap(int sock, char* file, int quiet) {
    close(sock);
    struct timeval start, end;
    char buffer[UDS_MAX], *data;
    int bytesReceived = 0, fd;
    fd = shm_open(file, O_RDONLY, 0644);
    if (fd == -1) {
        if(quiet == 0) perror("smh_open");
        exit(EXIT_FAILURE);
    }
    data = mmap(NULL, UDS_MAX, PROT_READ, MAP_SHARED, fd, 0);
    if(data == MAP_FAILED) {
        if(quiet == 0) perror("mmap");
        exit(EXIT_FAILURE);
    }

    gettimeofday(&start, NULL);
    do {
        snprintf(buffer, UDS_MAX, "%s", (char*)data);
        bytesReceived += strlen(buffer);
    } while (bytesReceived < 100 * 1024 * 1024);
    gettimeofday(&end, NULL);

    munmap(data, UDS_MAX);
    close(fd);

    return ((double)((end.tv_sec - start.tv_sec) * 1000000 + end.tv_usec - start.tv_usec)) / 1000000;
}

void printHelp() {
    printf("stnc:\n\nUsage: ./stnc [client/server] <parameters>\n\nRUN SERVER BEFORE CLIENT!\n\nFor server:\n ./stnc -s <PORT> <flag> <flag>\noptions:\n    -p      performance test\n    -q      quiet mode\n\n\nFor client:\n ./stnc -c <IP> <PORT> <flag> <type> <param>\nflag options:\n    -p      performance test\n\ntype options:\n     ipv4\n     ipv6\n     uds\n     mmap\n     pipe\n\nparam options:\n     tcp\n     udp\n     dgram\n     stream\n     filename\n");
}