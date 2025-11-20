#ifndef HANDLER_H
#define HANDLER_H

#include "net.h" 
#include <pthread.h>
void writeLog(const char *clientIP, const char *username, const char *action, const char *status);
void readFromFile(const char* filename);
const char* handleLogin(char *args, ClientState *client);
const char* handleLogout(ClientState *client);
int checkAuth(ClientState *client);

#endif