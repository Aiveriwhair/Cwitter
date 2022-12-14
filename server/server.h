#ifndef SERVEUR_H
#define SERVEUR_H

#include "client.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>

#define PORT 5000
#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
#define PUBLICATION_SIZE 20
typedef int SOCKET;

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

static clientList *clients;

/********   SERVER  ********/
int init_server(int port);
int start_server(int port);
void kill_server(SOCKET server_socket);

/********   REQUEST HANDLING  ********/
void handle_request(fd_set readfds, int server_socket);

void handle_subscribe(char *buffer, int client_socket);
void handle_unsubscribe(char *buffer, int client_socket);
void handle_publish(char *buffer, int client_socket);
void handle_list(char *buffer, int client_socket);
void handle_quit(char *buffer, int client_socket);
void handle_new_account(char *buffer, int client_socket);
void handle_login(char *buffer, int client_socket);

/********   SERVER-CLIENT COMMUNICATION  ********/
// void send_message(int client_socket, char *message);
// void send_message_to_list(int client_socket, char *message, clientList *list);

/********   DATA SAVE/LOAD  ********/

/*
DATA FORMAT :
N-Users
pseudo1
SubbedTo pseudo list
n-messages
message1
receiver1 receiver2 ...
message2
receiver1 receiver2 ...
...
pseudo2
...

*/

/***********    DATABASE   ***********/
void save_as(FILE *f, char *data);

void DB_save(char *file_path, clientList *Users);
clientList *DB_load(char *file_path);

/***********    TESTS   ***********/
void testDB_save(char *fpath, clientList *Users);
clientList *testDB_load(char *fpath);
clientList *clientList_tostring_test(bool prints);
messageList *messageList_tostring_test(bool prints);
Client *client_tostring_test(bool prints);
clientList *clients_tostring_test(bool prints);

#endif