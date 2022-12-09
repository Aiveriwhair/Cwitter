#include "client.h"

/*********  CLIENT  **********/
Client *init_client(SOCKET socket, char *name, messageList *messages, clientList *subbedTo)
{
    Client *client = malloc(sizeof(Client));
    client->socket = socket;
    strcpy(client->name, name);
    client->messages = messages;
    client->subbedTo = subbedTo;
    client->isConnected = false;
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
int messageList_length(messageList *messages)
{
    int length = 0;
    messageList *temp = messages;
    while (temp != NULL)
    {
        length++;
        temp = temp->next;
    }
    return length;
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

char *client_to_string(Client *client)
{
    char *res = strcat(client->name, "\n");
    res = strcat(res, clientList_to_string(client->subbedTo));
    res = strcat(res, "\n");
    res = strcat(res, messageList_to_string(client->messages));
    res = strcat(res, "\n");
    return res;
}
char *clients_to_string(clientList *clist)
{
    clientList *temp = clist;
    char *res = "";
    while (temp != NULL)
    {
        Client *current = temp->client;
        res = strcat(res, client_to_string(current));
        temp = temp->next;
    }
}

char *clientList_to_string(clientList *clist)
{
    char *res = "";
    clientList *temp = clist;
    while (temp != NULL)
    {
        Client *current = temp->client;
        res = strcat(strcat(res, current->name), " ");
        temp = temp->next;
    }
    return res;
}

char *messageList_to_string(messageList *messages)
{
    char *res = "";
    messageList *temp = messages;
    // n \n
    strcat(res, strcat(messageList_length(temp), "\n"));
    while (temp != NULL)
    {
        // message \n
        res = strcat(strcat(res, temp->content), "\n");
        // receivers \n
        res = strcat(res, clientList_to_string(temp->receivers));
        strcat(res, "\n");

        temp = temp->next;
    }
    return res;
}

int main(void)
{
    printf("Hello World!\n");
    return 0;
}