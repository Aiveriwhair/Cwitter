#include "client.h"

/*********  CLIENT  **********/
Client *init_client(SOCKET socket, char *name, messageList *messages, clientList *subbedTo)
{
    Client *client = malloc(sizeof(Client));
    client->socket = socket;
    strcpy(client->name, name);
    client->messages = messages;
    client->subbedTo = subbedTo;
    return client;
}
void delete_client(Client *client)
{
    delete_clientList(client->subbedTo);
    delete_messageList(client->messages);
    free(client);
}

void client_subscribe(Client *client, Client *SubscribeTo)
{
    add_client(client->subbedTo, SubscribeTo);
}

void client_unsubscribe(Client *client, Client *UnsubscribeTo)
{
    remove_client(client->subbedTo, UnsubscribeTo);
}

/*********  messageList  **********/

messageList *init_messageList(char *content, clientList *receivers)
{
    messageList *message = malloc(sizeof(messageList));
    message->content = content;
    message->receivers = receivers;
    message->next = NULL;
    return message;
}
void delete_messageList(messageList *messages)
{
    messageList *temp = messages;
    while (temp != NULL)
    {
        messageList *next = temp->next;
        delete_clientList(temp->receivers);
        free(temp);
        temp = next;
    }
}

void add_message(messageList *messages, char *message, clientList *receivers)
{
    messageList *temp = messages;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = init_messageList(message, receivers);
}
void remove_message(messageList *messages, char *message)
{
    messageList *temp = messages;
    if (temp->content == message)
    {
        messageList *next = temp->next;
        delete_messageList(temp);
        messages = next;
        return;
    }
    while (temp->next != NULL)
    {
        if (temp->next->content == message)
        {
            messageList *next = temp->next->next;
            delete_messageList(temp->next);
            temp->next = next;
            return;
        }
        temp = temp->next;
    }
}

/*********  CLIENTLIST  **********/
clientList *init_clienList(Client *client)
{
    clientList *subs = malloc(sizeof(clientList));
    subs->client = client;
    subs->next = NULL;
    return subs;
}

void delete_clientList(clientList *cList)
{
    clientList *temp = cList;
    while (temp != NULL)
    {
        clientList *next = temp->next;
        delete_client(temp->client);
        free(temp);
        temp = next;
    }
}

void add_client(clientList *cList, Client *client)
{
    clientList *temp = cList;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = init_clienList(client);
}

void remove_client(clientList *cList, Client *client)
{
    clientList *temp = cList;
    if (temp->client == client)
    {
        clientList *next = temp->next;
        delete_clientList(temp);
        cList = next;
        return;
    }
    while (temp->next != NULL)
    {
        if (temp->next->client == client)
        {
            clientList *next = temp->next->next;
            delete_clientList(temp->next);
            temp->next = next;
            return;
        }
        temp = temp->next;
    }
}

int main(void)
{
    printf("Hello World!\r");
    return 0;
}