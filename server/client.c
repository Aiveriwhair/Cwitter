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

int clientList_length(clientList *clist)
{
    int length = 0;
    clientList *temp = clist;
    while (temp != NULL)
    {
        length++;
        temp = temp->next;
    }
    return length;
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

void add_message(messageList *mlist, char *message, clientList *receivers)
{
    if (mlist == NULL)
    {
        mlist = init_messageList(message, receivers);
        return;
    }
    if (mlist->content == NULL)
    {
        mlist->content = message;
        mlist->receivers = receivers;
        return;
    }
    messageList *temp = mlist;
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
clientList *init_clientList(Client *client)
{
    clientList *clist = malloc(sizeof(clientList));
    clist->client = client;
    clist->next = NULL;
    return clist;
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
    if (cList == NULL)
    {
        cList = init_clientList(client);
        return;
    }
    if (cList->client == NULL)
    {
        cList->client = client;
        return;
    }
    clientList *temp = cList;
    while (temp->next != NULL)
    {
        temp = temp->next;
    }
    temp->next = init_clientList(client);
}

void remove_client(clientList *cList, Client *clientToUnsub)
{
    clientList *temp = cList;

    while (temp != NULL)
    {
        if (temp->client == clientToUnsub)
        {
            clientList *next = temp->next;
            free(temp);
            cList = next;
            return;
        }
        temp = temp->next;
    }
}

char *client_to_string(Client *client)
{
    char *res = malloc(0);
    // pseudo
    if (client->name != NULL)
    {
        res = realloc(res, charS * strlen(client->name) + charS);
        res = strcat(strcat(res, client->name), "\n");
    }
    else
    {
        res = realloc(res, charS * strlen(client->name) + charS);
        res = strcat(res, "unamed\n");
    }

    // receivers \n
    if (client->subbedTo != NULL)
    {
        char *subbedtoStr = clientList_to_string(client->subbedTo);
        res = realloc(res, charS * strlen(res) + charS * strlen(subbedtoStr) + charS);
        res = strcat(res, subbedtoStr);
    }

    // messages
    if (client->messages != NULL)
    {
        char *messagesStr = messageList_to_string(client->messages);
        res = realloc(res, charS * strlen(res) + charS * strlen(messagesStr) + charS);
        res = strcat(res, messagesStr);
    }
    else
    {
        char *messagesStr = "0\n";
        res = realloc(res, charS * strlen(res) + charS * strlen(messagesStr) + charS);
        res = strcat(res, messagesStr);
    }
    return res;
}
char *clients_to_string(clientList *clist)
{
    clientList *temp = clist;
    char *res = malloc(0);
    while (temp != NULL)
    {
        Client *current = temp->client;
        if (current == NULL)
            break;
        char *currentStr = client_to_string(current);
        res = realloc(res, charS * strlen(res) + charS * strlen(currentStr) + charS);
        res = strcat(res, currentStr);
        temp = temp->next;
    }
    return res;
}

char *clientList_to_string(clientList *clist)
{
    char *res = malloc(0);
    clientList *temp = clist;
    while (temp != NULL)
    {
        Client *current = temp->client;
        // printf("realloc %d bytes\n", charS * strlen(res) + charS * strlen(current->name) + charS);
        res = (char *)realloc(res, charS * strlen(res) + charS * strlen(current->name) + charS);
        if (temp->next == NULL)
        {
            strcat(strcat(res, current->name), "\n");
        }
        else
        {
            strcat(strcat(res, current->name), " ");
        }
        temp = temp->next;
    }
    return res;
}

char *messageList_to_string(messageList *messages)
{
    messageList *temp = messages;
    // n \n
    char *res = (char *)malloc(intlen(messageList_length(temp)) + 1);
    strcat(strcat(res, intToString(messageList_length(temp))), "\n");
    while (temp != NULL)
    {
        // message \n
        res = realloc(res, charS * strlen(res) + charS * strlen(temp->content) + charS);
        res = strcat(strcat(res, temp->content), "\n");

        // receivers \n
        char *receiversStr = clientList_to_string(temp->receivers);
        res = realloc(res, charS * strlen(res) + charS * strlen(receiversStr) + charS);
        res = strcat(res, receiversStr);

        temp = temp->next;
    }
    return res;
}

char *intToString(int n)
{
    int len = intlen(n);
    char *res = malloc(len + 1);
    sprintf(res, "%d", n);
    return res;
}

int intlen(int n)
{
    return floor(log10(abs(n))) + 1;
}