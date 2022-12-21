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
#include <termios.h> 
#include <stdbool.h>

#define BUFFER_SIZE 1024
#define SOCKET int

enum requestType
{
    LIST = 1,
    SUBSCRIBE = 2,
    UNSUBSCRIBE = 3,
    PUBLISH = 4,
    QUIT = 5,
    NEW_ACCOUNT = 6,
    LOGIN = 7,
};

/**********     CLIENT    **********/
char* auth();
void start_client(char *ip, int port, char *pseudo);
int init_client(char *ip, int port);
void kill_client(SOCKET client_socket);

/**********     TO SERVER    **********/
void write_to_server(SOCKET client_socket, char *buffer);
void handle_list(SOCKET client_socket, char *buffer);
void handle_subscribe(SOCKET client_socket, char *buffer);
void handle_unsubscribe(SOCKET client_socket, char *buffer);
void handle_publish(SOCKET client_socket, char *buffer);




/**********     REQUEST HANDLING    **********/
int request_server(SOCKET client_socket, char *buffer);
int receive_server(SOCKET client_socket, char *buffer);
void handle_error(char *buffer);

/**********     UI    **********/
void print_welcome();
void print_menu();

#endif
