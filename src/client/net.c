#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "net.h"

int getLine(char* mainBuffer, char* lineOut) {
    char* newlinePos = strstr(mainBuffer, "\n");
    if (newlinePos == NULL) return 0;

    int lineLen = newlinePos - mainBuffer;
    strncpy(lineOut, mainBuffer, lineLen);
    lineOut[lineLen] = '\0';

    if (lineLen > 0 && lineOut[lineLen - 1] == '\r') {
        lineOut[lineLen - 1] = '\0';
    }

    memmove(mainBuffer, newlinePos + 1, strlen(newlinePos + 1) + 1);
    return 1;
}

void sendResponse(int socket, const char* data) {
    write(socket, data, strlen(data));
}

int createConnection(const char* ip, int port) {
    int sock;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, ip, &serv_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }

    return sock;
}