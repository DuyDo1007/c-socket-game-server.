#ifndef NET_H
#define NET_H

#include <netinet/in.h>

#define BUFFER_SIZE 2048

int getLine(char* mainBuffer, char* lineOut);

void sendResponse(int socket, const char* data);

int createConnection(const char* ip, int port);

#endif