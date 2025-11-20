#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "net.h"

char recvBuffer[BUFFER_SIZE] = {0};

void receiveResponse(int sock, char *responseCode) {
    char tempBuffer[BUFFER_SIZE];
    int n;

    while (getLine(recvBuffer, responseCode) == 0) {
        n = read(sock, tempBuffer, sizeof(tempBuffer) - 1);
        if (n <= 0) {
            printf("Server disconnected.\n");
            exit(1);
        }
        tempBuffer[n] = '\0';
        strcat(recvBuffer, tempBuffer);
    }
}

void handleServerMessage(const char *code) {
    printf("\n >> Code(%s): ", code);
    if (strcmp(code, "100") == 0) printf("Connected to server successfully.\n");
    else if (strcmp(code, "110") == 0) printf("Login successful!\n");
    else if (strcmp(code, "120") == 0) printf("Message posted successfully.\n");
    else if (strcmp(code, "130") == 0) printf("Logged out successfully.\n");
    else if (strcmp(code, "211") == 0) printf("Error: Account is banned/blocked.\n");
    else if (strcmp(code, "212") == 0) printf("Error: Wrong username or password.\n");
    else if (strcmp(code, "213") == 0) printf("Error: You are already logged in on this device.\n");
    else if (strcmp(code, "214") == 0) printf("Error: This account is already logged in on another device.\n");
    else if (strcmp(code, "221") == 0) printf("Error: You must be logged in to do this.\n");
    else if (strcmp(code, "300") == 0) printf("Error: Invalid command syntax.\n");
    else printf("Unknown response.\n");
    printf("------------------------------------------------\n");
}
int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <Server_IP> <Port>\n", argv[0]);
        return 1;
    }
    
    int sock = createConnection(argv[1], atoi(argv[2]));
    if (sock < 0) return 1;

    char response[BUFFER_SIZE];
    
    receiveResponse(sock, response);
    handleServerMessage(response);

    int choice;
    char buffer[BUFFER_SIZE];
    char username[100], password[100], message[200];

    while (1) {
        printf("\n=== MENU ===\n");
        printf("1. Login\n");
        printf("2. Post Message\n");
        printf("3. Logout\n");
        printf("4. Exit\n");
        printf("Choice: ");
        
        if (scanf("%d", &choice) != 1) {
            while(getchar() != '\n'); 
            continue;
        }
        while(getchar() != '\n'); 

        switch (choice) {
            case 1: 
                printf("Username: ");
                scanf("%s", username);
                printf("Password: ");
                scanf("%s", password);
                while(getchar() != '\n'); 

                snprintf(buffer, sizeof(buffer), "USER %s %s\n", username, password);
                sendResponse(sock, buffer);
                
                receiveResponse(sock, response);
                handleServerMessage(response);
                break;

            case 2: 
                printf("Enter message: ");
                fgets(message, sizeof(message), stdin);
                message[strcspn(message, "\n")] = 0; 

                snprintf(buffer, sizeof(buffer), "POST %s\n", message);
                sendResponse(sock, buffer);

                receiveResponse(sock, response);
                handleServerMessage(response);
                break;

            case 3: 
                sendResponse(sock, "BYE\n");
                
                receiveResponse(sock, response);
                handleServerMessage(response);
                break;

            case 4: 
                printf("Exiting...\n");
                close(sock);
                return 0;

            default:
                printf("Invalid choice.\n");
        }
    }
    return 0;
}