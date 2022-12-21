#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>

#define PSEUDO_SIZE 7
#define charS sizeof(char)
#define intS sizeof(int)
typedef int SOCKET;

typedef struct clientList
{
    struct Client *client;
    struct clientList *next;
} clientList;

typedef struct messageList
{
    char *content;
    struct clientList *receivers;
    struct messageList *next;
} messageList;

typedef struct Client
{
    SOCKET socket;
    char name[PSEUDO_SIZE];
    bool isConnected;
    clientList *subbedTo;
    messageList *messages;
} Client;

Client *init_client(SOCKET socket, char *name, messageList *messages, clientList *subbedTo);
void delete_client(Client *client);
void client_subscribe(Client *client, Client *SubscribeTo);
void client_unsubscribe(Client *client, Client *UnsubscribeTo);

messageList *init_messageList(char *content, clientList *receivers);
void delete_messageList(messageList *messages);
void add_message(messageList *messages, char *message, clientList *receivers);
void remove_message(messageList *messages, char *message);
int messageList_length(messageList *messages);

clientList *init_clientList(Client *client);
void delete_clientList(clientList *clist);
void add_client(clientList *clist, Client *client);
void remove_client(clientList *clientList, Client *client);
int clientList_length(clientList *clist);

char *client_to_string(Client *client);
char *clients_to_string(clientList *clients);
char *clientList_to_string(clientList *clist);
char *messageList_to_string(messageList *messages);

/***********    OTHER   ***********/
int intlen(int n);
char *intToString(int n);
#endif