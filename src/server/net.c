#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include "net.h"

int getLine(char* mainBuffer, char* lineOut) {
    char* newlinePos = strstr(mainBuffer, "\n");
    if (newlinePos == NULL) {
        return 0; 
    }

    int lineLen = newlinePos - mainBuffer;
    strncpy(lineOut, mainBuffer, lineLen);
    lineOut[lineLen] = '\0';
    if (lineLen > 0 && lineOut[lineLen - 1] == '\r') {
        lineOut[lineLen - 1] = '\0';
    }

    memmove(mainBuffer, newlinePos + 1, strlen(newlinePos + 1) + 1);
    return 1;
}

void sendResponse(int socket, const char* responseCode) {
    char buffer[256];
    snprintf(buffer, sizeof(buffer), "%s\n", responseCode);
    write(socket, buffer, strlen(buffer));
}