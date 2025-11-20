#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <errno.h>

#include "net.h"
#include "handler.h"

ClientState clients[MAX_CLIENTS];

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        return 1;
    }

    readFromFile("../../data/account.txt");
    int server_fd, new_socket, max_sd, sd, activity;
    struct sockaddr_in address;
    fd_set readfds; 

    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].socketFd = 0;
    }

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Socket failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(atoi(argv[1]));

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 10) < 0) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    printf("Server running on port %s...\n", argv[1]);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_fd, &readfds);
        max_sd = server_fd;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = clients[i].socketFd;
            if (sd > 0) FD_SET(sd, &readfds);
            if (sd > max_sd) max_sd = sd;
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR)) {
            printf("Select error\n");
        }

        if (FD_ISSET(server_fd, &readfds)) {
            int addrlen = sizeof(address);
            if ((new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("Accept");
                continue;
            }

            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].socketFd == 0) {
                    clients[i].socketFd = new_socket;
                    clients[i].address = address;
                    clients[i].isLoggedIn = 0;
                    clients[i].username[0] = '\0';
                    clients[i].recvBuffer[0] = '\0';
                    
                    printf("New connection, socket fd is %d\n", new_socket);
                    writeLog(inet_ntoa(address.sin_addr), "GUEST", "CONNECT", "Success");
                    sendResponse(new_socket, "100");
                    break;
                }
            }
        }

        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = clients[i].socketFd;
            ClientState *cli = &clients[i];

            if (FD_ISSET(sd, &readfds)) {
                char buffer[1024];
                int valread = read(sd, buffer, 1024);

                if (valread == 0) {
                    getpeername(sd, (struct sockaddr*)&address, (socklen_t*)&(int){sizeof(address)});
                    printf("Host disconnected, ip %s\n", inet_ntoa(address.sin_addr));
                    writeLog(inet_ntoa(address.sin_addr), cli->username, "DISCONNECT", "Success");
                    close(sd);
                    cli->socketFd = 0;
                } else {
                    buffer[valread] = '\0';
                    
                    if (strlen(cli->recvBuffer) + valread < BUFFER_SIZE) {
                        strcat(cli->recvBuffer, buffer);
                    }

                    char line[BUFFER_SIZE];
                    while (getLine(cli->recvBuffer, line) == 1) {
                        printf("Client %d sent: %s\n", sd, line);
                        
                        char command[20], args[200] = {0};
                        sscanf(line, "%s %[^\n]", command, args); 

                        const char* res = "300";

                        if (strcmp(command, "USER") == 0) { 
                            res = handleLogin(args, cli);
                            writeLog(inet_ntoa(cli->address.sin_addr), args, "LOGIN", res);
                        } 
                        else if (strcmp(command, "BYE") == 0) {
                            res = handleLogout(cli);
                        }
                        else {
                            if (!checkAuth(cli)) {
                                res = "221";
                            } else {
                                if (strcmp(command, "POST") == 0) {
                                    res = "120";
                                }
                            }
                        }
                        sendResponse(sd, res);
                    }
                }
            }
        }
    }
    return 0;
}