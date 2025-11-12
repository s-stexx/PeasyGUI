// Simple cross-platform TCP client helper for PeasyGUI.  
// Handles socket setup, connection, and request/response exchange.  
// Keeps the main C code short and readable.

#ifndef TCP_CONNECTION_H
#define TCP_CONNECTION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#ifdef _WIN32
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")

    // Windows doesn't have close() for sockets
    #define close closesocket

    // Windows needs WSAStartup/WSACleanup
    static void init_sockets() {
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2,2), &wsa) != 0) {
            fprintf(stderr, "WSAStartup failed\n");
            exit(1);
        }
    }
    static void cleanup_sockets() {
        WSACleanup();
    }

#else
    #include <unistd.h>         // for close(), usleep()
    #include <arpa/inet.h>      // for sockaddr_in, inet_addr
    #include <sys/socket.h>     // for socket(), connect()
    #include <netinet/in.h>     // for struct sockaddr_in

    static void init_sockets() { /* nothing needed */ }
    static void cleanup_sockets() { /* nothing needed */ }
#endif


// request() sends a command and returns a malloc'ed reply string.
// Caller must free() the returned string.
static char* request(const char *command, int sock) {
    char buffer[100000];
    ssize_t n;

    // sendall equivalent (best effort)
    size_t total_sent = 0;
    size_t len = strlen(command);
    while (total_sent < len) {
        ssize_t sent = send(sock, command + total_sent, (int)(len - total_sent), 0);
        if (sent <= 0) {
            perror("send");
            return NULL;
        }
        total_sent += sent;
    }

    // receive
    n = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (n <= 0) {
        perror("recv");
        return NULL;
    }
    buffer[n] = '\0';

    // check ERROR`
    if (strncmp(buffer, "ERROR`", 6) == 0) {
        fprintf(stderr, "Server error: %s\n", buffer);
        return NULL;
    }

    // copy reply to heap
    char *reply;
    if (strncmp(command, "get`", 4) == 0) {
        // strip first 4 chars and trailing backtick
        size_t rlen = (size_t)n - 4; // skip 3 + last '`'
        reply = (char*)malloc(rlen + 1);
        if (!reply) return NULL;
        strncpy(reply, buffer + 3, rlen);
        reply[rlen] = '\0';
    } else {
        #ifdef _WIN32
            reply = _strdup(buffer);
        #else
            reply = strdup(buffer);
        #endif
    }

    return reply;
}


// connect_to_gui() tries until connection is successful.
// Returns socket file descriptor.
static int connect_to_gui() {
    int sock;
    struct sockaddr_in server;
    int connected = 0;

    init_sockets();

    while (!connected) {
    #ifdef _WIN32
        Sleep(100); // ms
    #else
        usleep(100000); // 0.1s
    #endif

        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0) {
            perror("socket");
            continue;
        }

        memset(&server, 0, sizeof(server));
        server.sin_family = AF_INET;
        server.sin_port = htons(10000);
        server.sin_addr.s_addr = inet_addr("127.0.0.1");

        if (connect(sock, (struct sockaddr *)&server, sizeof(server)) == 0) {
            connected = 1;
        } else {
            printf("Try to connect ...\n");
            close(sock);
        }
    }

    return sock;
}

#endif // TCP_CONNECTION_H
