#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "handler.h"

#define MAX_USER 100
#define MAX_LOGGED_IN_USERS 100

struct User {
    char username[100];
    char password[100]; 
    int status;         
} users[MAX_USER];

int userCount = 0;

char loggedInUsers[MAX_LOGGED_IN_USERS][100];
int loggedInCount = 0;
pthread_mutex_t loginUser = PTHREAD_MUTEX_INITIALIZER;

int isUserLoggedIn(const char* username) {
    for (int i = 0; i < loggedInCount; i++) {
        if (strcmp(loggedInUsers[i], username) == 0) {
            return 1; 
        }
    }
    return 0; 
}

void addLoggedInUser(const char* username) {
    if (loggedInCount < MAX_LOGGED_IN_USERS) {
        strcpy(loggedInUsers[loggedInCount], username);
        loggedInCount++;
    }
}

void removeLoggedInUser(const char* username) {
    for (int i = 0; i < loggedInCount; i++) {
        if (strcmp(loggedInUsers[i], username) == 0) {
            strcpy(loggedInUsers[i], loggedInUsers[loggedInCount - 1]);
            loggedInCount--;
            break;
        }
    }
}

void writeLog(const char *clientIP, const char *username, const char *action, const char *status) {
    FILE *file = fopen("../../logs/server_log.txt", "a");
    if (file == NULL) return;

    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    char timestamp[30];
    strftime(timestamp, sizeof(timestamp), "[%d/%m/%Y %H:%M:%S]", local);

    fprintf(file, "%s %s (%s) $ %s $ %s\n", timestamp, clientIP, username, action, status);
    fclose(file);
}
void readFromFile(const char* filename) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening account.txt");
        return;
    }
    userCount = 0;
    while (fscanf(file, "%s %s %d", users[userCount].username, users[userCount].password, &users[userCount].status) != EOF) {
        userCount++;
    }
    fclose(file);
    printf("Loaded %d accounts.\n", userCount);
}
const char* handleLogin(char *args, ClientState *client) {
    char username[100], password[100];
    
    if (sscanf(args, "%s %s", username, password) != 2) {
        return "300"; 
    }

    if (client->isLoggedIn) return "213"; 

    pthread_mutex_lock(&loginUser);

    if (isUserLoggedIn(username)) {
        pthread_mutex_unlock(&loginUser);
        return "214";
    }

    int found = 0;
    const char* response = "212";

    for (int i = 0; i < userCount; i++) {
        if (strcmp(users[i].username, username) == 0) {
            found = 1;
            if (strcmp(users[i].password, password) == 0) {
                if (users[i].status == 1) {
                    addLoggedInUser(username);
                    
                    client->isLoggedIn = 1;
                    strcpy(client->username, username);
                    response = "110";
                } else {
                    response = "211";
                }
            } else {
                response = "212";
            }
            break; 
        }
    }
    
    pthread_mutex_unlock(&loginUser);
    
    return response;
}

const char* handleLogout(ClientState *client) {
    if (!client->isLoggedIn) return "221";
    pthread_mutex_lock(&loginUser);
    removeLoggedInUser(client->username);
    pthread_mutex_unlock(&loginUser);

    client->isLoggedIn = 0;
    client->username[0] = '\0';
    
    return "130";
}

int checkAuth(ClientState *client) {
    return client->isLoggedIn;
}