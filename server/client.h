#ifndef CLIENT_H
#define CLIENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PSEUDO_SIZE 6
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

clientList *init_clienList(Client *client);
void delete_clientList(clientList *clientList);
void add_client(clientList *clientList, Client *client);
void remove_client(clientList *clientList, Client *client);
#endif