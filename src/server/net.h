#ifndef NET_H
#define NET_H

#include <netinet/in.h>

#define BUFFER_SIZE 2048
#define MAX_CLIENTS 1024
typedef struct {
    int socketFd;
    struct sockaddr_in address;
    int isLoggedIn; 
    char username[100];
    char recvBuffer[BUFFER_SIZE]; 
} ClientState;

int getLine(char* mainBuffer, char* lineOut);

void sendResponse(int socket, const char* responseCode);

#endif