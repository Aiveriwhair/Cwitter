#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define SOCKET int

enum requestType
{
    SUBSCRIBE = 0,
    UNSUBSCRIBE = 1,
    PUBLISH = 2,
    LIST = 3,
    QUIT = 4,
    NEW_ACCOUNT = 5,
    LOGIN = 6,
};
void run();

/**********     CLIENT    **********/
void start_client(char *ip, int port);
int init_client(char *ip, int port);
void kill_client(SOCKET client_socket);

/**********     REQUEST HANDLING    **********/
int request_server(SOCKET client_socket, char *buffer);
int receive_server(SOCKET client_socket, char *buffer);

/**********     UI    **********/
void print_welcome();
void print_menu();
#endif
