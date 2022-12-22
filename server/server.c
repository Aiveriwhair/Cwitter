#include "server.h"

#define check_error(expr, userMsg) \
    do                             \
    {                              \
        if (expr == -1)            \
        {                          \
            perror(userMsg);       \
            exit(errno);           \
        }                          \
    } while (0)

static clientList *clients;

int start_server(int port)
{
    printf("Starting server on port %d\n", port);
    SOCKET server_socket = init_server(port);

    int maxFD = server_socket;
    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(server_socket, &readfds);

    while (1)
    {
        printf("Waiting for new select resolution\n");

        for (int i = server_socket; i < maxFD + 1; i++)
        {
            FD_SET(i, &readfds);
        }

        int resultSelect = select(maxFD + 1, &readfds, NULL, NULL, NULL);
        check_error(resultSelect, "error in select()\n");
        printf("Select resolved\n");

        if (FD_ISSET(server_socket, &readfds))
        {
            printf("New client connection\n");
            struct sockaddr_in csin = {0};
            size_t sinsize = sizeof csin;
            SOCKET csock = accept(server_socket, (struct sockaddr *)&csin, &sinsize);
            check_error(csock, "error in accept()\n");

            printf("New client connected from %s:%d - socket %d\n", inet_ntoa(csin.sin_addr),
                   htons(csin.sin_port), csock);

            handle_request(readfds, csock);
            if (csock > maxFD)
            {
                maxFD = csock;
            }
        }
        else
        {
            printf("New request\n");
            for (int i = 0; i < maxFD + 1; i++)
            {
                if (FD_ISSET(i, &readfds))
                {
                    printf("Request from socket %d\n", i);
                    handle_request(readfds, i);
                }
            }
        }
    }
    kill_server(server_socket);
}

int init_server(int port)
{
    SOCKET server_socket = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sin;

    check_error(server_socket, "error in initialization socket()\n");

    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    sin.sin_port = htons(port);
    sin.sin_family = AF_INET;

    int resultBind = bind(server_socket, (struct sockaddr_in *)&sin, sizeof sin);
    check_error(resultBind, "error in bind()\n");

    int res = listen(server_socket, MAX_CLIENTS);
    check_error(res, "error in listen()\n");

    printf("Initialization of the server done\n");
    printf("Socket  n° %d\n", server_socket);

    return server_socket;
}

void handle_request(fd_set readfds, SOCKET server_socket)
{
    char *buffer = malloc(BUFFER_SIZE * sizeof(char));
    memset(buffer, '\0', BUFFER_SIZE);
    int resultRecv = recv(server_socket, buffer, BUFFER_SIZE - 1, 0);
    check_error(resultRecv, "error in handle_request recv()\n");
    printf("Request type : %c\n", buffer[0]);
    switch (buffer[0])
    {
    case '1':
        printf("LIST request\n");
        handle_list(buffer, server_socket);
        break;
    case '2':
        printf("SUBSCRIBE request\n");
        handle_subscribe(buffer, server_socket);
        break;
    case '3':
        printf("UNSUBSCRIBE request\n");
        handle_unsubscribe(buffer, server_socket);
        break;
    case '4':
        printf("PUBLISH request\n");
        handle_publish(buffer, server_socket);
        break;
    case '5':
        printf("QUIT request\n");
        handle_quit(buffer, server_socket);
        break;
    case '6':
        printf("NEW_ACCOUNT request\n");
        handle_new_account(buffer, server_socket);

        break;
    case '7':
        printf("LOGIN request\n");
        handle_login(buffer, server_socket);
        break;
    default:
        printf("Unknown request\n");
        break;
    }
    free(buffer);
    buffer = NULL;
}

void kill_server(SOCKET server_socket)
{
    close(server_socket);
}

void handle_subscribe(char *buffer, SOCKET client_socket)
{
    // Get current client
    Client *currentClient = get_client_by_socket(client_socket);
    if (currentClient == NULL)
    {
        printf("SERVER ERROR\n");
        return;
    }

    // Create response buffer
    char *response = malloc(BUFFER_SIZE);
    memset(response, '\0', BUFFER_SIZE);

    // Create sub name buffer & put name in it
    char *name = malloc(BUFFER_SIZE);
    memset(name, '\0', BUFFER_SIZE);
    name = strcpy(name, buffer + 1);

    // Check if already subscribed
    Client *sub = get_client_by_name(currentClient->subbedTo, name);
    // If yes, send error response
    if (sub != NULL)
    {
        printf("Client already subscribed to %s\n", name);
        // Send error response
        response[0] = 'e';
        response[1] = 'd';
        int n = send(client_socket, response, BUFFER_SIZE, 0);
        check_error(n, "error in handle_subs send()\n");
        free(name);
        free(response);
        return;
    }
    // If no, continue
    sub = get_client_by_name(clients, name);

    // Check if trying to sub to self
    if (sub == currentClient)
    {
        printf("Client tried to self subscribe\n");
        response[0] = 'e';
        response[1] = 's';
        int n = send(client_socket, response, BUFFER_SIZE, 0);
        check_error(n, "error in handle_subs send()\n");
        free(name);
        free(response);
        return;
    }

    // Check if sub name exists
    // If no, send error response
    if (sub == NULL)
    {
        printf("Client %s doesn't exist", name);
        // Send error response
        response[0] = 'e';
        response[1] = 'n';
        int n = send(client_socket, response, BUFFER_SIZE, 0);
        check_error(n, "error in handle_subs send()\n");
        free(name);
        free(response);
        return;
    }
    // If yes, add to subbedTo list
    if (currentClient->subbedTo == NULL)
    {
        currentClient->subbedTo = init_clientList(sub);
    }
    else
    {
        add_client(currentClient->subbedTo, sub);
    }
    printf("Printing subbedTo list :\n");
    printf("%s\n", clientList_to_string(currentClient->subbedTo));

    response[0] = '2';
    strcat(response + 1, name);
    int n = send(client_socket, response, BUFFER_SIZE, 0);
    check_error(n, "error in handle_subs send()\n");
    printf("Client subed to %s\n", name);

    free(name);
    free(response);
}

void handle_unsubscribe(char *buffer, SOCKET client_socket)
{
    // Get current client
    Client *currentClient = get_client_by_socket(client_socket);
    if (currentClient == NULL)
    {
        printf("SERVER ERROR\n");
        return;
    }

    // Create response buffer
    char *response = malloc(BUFFER_SIZE);
    memset(response, '\0', BUFFER_SIZE);

    // Create sub name buffer & put name in it
    char *name = malloc(BUFFER_SIZE);
    memset(name, '\0', BUFFER_SIZE);
    name = strcpy(name, buffer + 1);

    clientList *temp = clients;

    Client *sub = get_client_by_name(temp, name);
    // If no send error response
    if (sub == NULL)
    {
        printf("The client %s doesn't exist\n", name);
        // Send error response
        response[0] = 'e';
        response[1] = 'n';
        int n = send(client_socket, response, BUFFER_SIZE, 0);
        check_error(n, "error in handle_unsubs send()\n");
        free(name);
        free(response);
        return;
    }
    // If yes, continue
    // check if trying to unsub from self
    if (sub == currentClient)
    {
        printf("Client tried to self unsubscribe\n");
        response[0] = 'e';
        response[1] = 't';
        int n = send(client_socket, response, BUFFER_SIZE, 0);
        check_error(n, "error in handle_unsubs send()\n");
        free(name);
        free(response);
        return;
    }

    // Check if the client is in subbedTo list
    Client *cli = get_client_by_name(currentClient->subbedTo, name);
    // If no, send error response
    if (cli == NULL)
    {
        // Send error response
        printf("Client %s is not in subbedTo list\n", name);
        response[0] = 'e';
        response[1] = 'u';
        int n = send(client_socket, response, BUFFER_SIZE, 0);
        check_error(n, "error in handle_unsubs send()\n");
        free(name);
        free(response);
        return;
    }

    // If yes, remove from subbedTo list
    // Remove from subbedTo list
    remove_client(currentClient->subbedTo, cli);

    printf("Printing subbedTo list : ");
    printf("%s \n", clientList_to_string(currentClient->subbedTo));

    response[0] = '3';
    strcat(response + 1, name);
    printf("after strcat\n ");

    int n = send(client_socket, response, BUFFER_SIZE, 0);
    check_error(n, "error in handle_unsubs send()\n");
    printf("Client unsubed to %s\n", name);

    free(name);
    free(response);
}

void handle_publish(char *buffer, int client_socket)
{
    // le premier qui fini
}

void handle_list(char *buffer, SOCKET client_socket)
{
    if (clients == NULL)
    {
        printf("No clients connected\n");
        return;
    }

    char *response = malloc(sizeof(char) * BUFFER_SIZE);
    memset(response, '\0', BUFFER_SIZE);
    clientList *tmp = clients;
    response[0] = '1';
    response[1] = '-';
    for (tmp; tmp != NULL; tmp = tmp->next)
    {
        response = strcat(response, tmp->client->name);
        response = strcat(response, "-");
    }
    printf("buffer in list : %s\n", response);
    int n = send(client_socket, response, BUFFER_SIZE, 0);
    check_error(n, "error in handle_list send()\n");
}

void handle_quit(char *buffer, SOCKET client_socket)
{
    // Will
    // Set isConnected to 0
}

void handle_new_account(char *buffer, SOCKET client_socket)
{

    // add check of existant username

    buffer = buffer + 1;
    printf("buffer : %s\n", buffer);
    char *name = malloc(sizeof(char) * BUFFER_SIZE);
    name = strcpy(name, buffer);
    printf("name : %s\n", name);
    Client *newClient = init_client(client_socket, name, NULL, NULL);
    newClient->isConnected = 1;
    if (clients == NULL)
    {
        printf("clients is null\n");
        clients = init_clientList(newClient);
    }
    else
    {
        printf("clients is not null\n");
        add_client(clients, newClient);
    }

    clientList *temp = clients;
    printf("clients: \n");
    for (temp; temp != NULL; temp = temp->next)
    {
        printf("\t%s\n", temp->client->name);
    }
    free(name);
}

void handle_login(char *buffer, SOCKET client_socket)
{
    // Will
    char *response = malloc(sizeof(char) * BUFFER_SIZE);

    Client *current;
    // Check if user exists
    buffer = buffer + 1;
    char *name = malloc(sizeof(char) * BUFFER_SIZE);
    name = strcpy(name, buffer);
    // current = get_client_by_name(name);

    if (current == NULL) // If no,
    {
        // Send error message
        printf("Client %s doesn't exist\n", name);
        response[0] = 'e';
        response[1] = 'l';
        int n = send(client_socket, response, BUFFER_SIZE, 0);
        check_error(n, "error in handle_error send()\n");
    }
    else // If yes,
    {
        // Set isConnected to 1
        current->isConnected = 1;
        // Set client socket
        current->socket = client_socket;
        // Send success message
        printf("Client %s logged in\n", name);
    }
    free(name);
}

Client *get_client_by_socket(SOCKET client_socket)
{
    clientList *clientsList = clients;
    while (clientsList != NULL)
    {
        if (clientsList->client->socket == client_socket)
        {
            return clientsList->client;
        }
        clientsList = clientsList->next;
    }
    return NULL;
}

Client *get_client_by_name(clientList *cList, char *name)
{
    clientList *clientsList = cList;
    while (clientsList != NULL)
    {
        if (strcmp(clientsList->client->name, name) == 0)
        {
            return clientsList->client;
        }
        clientsList = clientsList->next;
    }
    return NULL;
}

// Client *get_client_by_name(char *name)
// {
//     clientList *clientsList = clients;
//     while (clientsList != NULL)
//     {
//         if (strcmp(clientsList->client->name, name) == 0)
//         {
//             return clientsList->client;
//         }
//         clientsList = clientsList->next;
//     }
//     return NULL;
// }

void save_as(FILE *file, char *data)
{

    fprintf(file, "%s", data);

    fclose(file);
    free(data);
}

clientList *loadDB_from(char *fpath)
{
    clientList *clist = init_clientList(NULL);

    FILE *file = fopen(fpath, "r+");
    if (file == NULL)
    {
        printf("Error opening file");
    }
    int users_number;
    fscanf(file, "%d", &users_number);
    for (int i = 0; i < users_number; i++)
    {
        char *pseudo;
        // Récuperer le pseudo
        // Récuperer les abonnements
        // Récupérer le nombre de messages
        // Pour chaque message
        // Récuperer le message
        // Récuperer la liste des receivers
    }

    fclose(file);
    return clist;
}

void DB_save(char *file_path, clientList *Users)
{
    FILE *file = fopen(file_path, "w+");
    if (file == NULL)
    {
        printf("Error opening file");
    }
    fprintf(file, "%d\n", clientList_length(Users));
    char *data = clients_to_string(Users);
    save_as(file, data);
}
void testDB_save(char *fpath, clientList *Users)
{
    if (Users == NULL)
    {
        clientList *receivers = clientList_tostring_test(false);
        messageList *mlist = messageList_tostring_test(false);
        clientList *allUsers = init_clientList(NULL);
        add_client(allUsers, init_client(0, "Willia", mlist, receivers));
        add_client(allUsers, init_client(0, "Lea", NULL, receivers));
        add_client(allUsers, init_client(0, "Margot", NULL, NULL));
        add_client(allUsers, init_client(0, "Jerem", NULL, receivers));
        add_client(allUsers, init_client(0, "Lucas", mlist, NULL));
        add_client(allUsers, init_client(0, "Benja", mlist, NULL));
        add_client(allUsers, init_client(0, "Lony", NULL, NULL));
        add_client(allUsers, init_client(0, "Luiza", NULL, receivers));

        char *data = clients_to_string(allUsers);
        char *file_path = fpath;

        save_as(file_path, data);
    }
    else
    {
        char *data = clients_to_string(Users);
        char *file_path = fpath;
        save_as(file_path, data);
    }
}

clientList *testDB_load(char *fpath)
{
    // return load_from(fpath);
}

clientList *clientList_tostring_test(bool prints)
{
    clientList *clist = init_clientList(NULL);
    add_client(clist, init_client(0, "Willia", NULL, NULL));
    add_client(clist, init_client(0, "Margot", NULL, NULL));
    add_client(clist, init_client(0, "Lucas", NULL, NULL));
    add_client(clist, init_client(0, "Jeremy", NULL, NULL));
    add_client(clist, init_client(0, "Benjam", NULL, NULL));
    if (prints)
    {
        printf("clientList_tostring_test : %s", clientList_to_string(clist));
    }
    return clist;
}

messageList *messageList_tostring_test(bool prints)
{
    clientList *receivers = clientList_tostring_test(false);
    messageList *mlist = init_messageList(NULL, NULL);
    add_message(mlist, "Message1", receivers);
    add_message(mlist, "Message2", receivers);
    add_message(mlist, "Message3", receivers);
    add_message(mlist, "Message4", receivers);
    if (prints)
    {
        char *str = messageList_to_string(mlist);
        printf("messageList_tostring_test : %s", str);
        free(str);
    }
    return mlist;
}

Client *client_tostring_test(bool prints)
{
    clientList *receivers = clientList_tostring_test(false);
    messageList *mlist = messageList_tostring_test(false);
    Client *cli = init_client(0, "Willia", mlist, receivers);
    if (prints)
    {
        char *str = client_to_string(cli);
        printf("clientTest:\n%s", str);
        free(str);
    }
    return cli;
}
clientList *clients_tostring_test(bool prints)
{
    clientList *clist = init_clientList(NULL);
    add_client(clist, client_tostring_test(false));
    add_client(clist, client_tostring_test(false));
    add_client(clist, client_tostring_test(false));
    if (prints)
    {
        char *str = clients_to_string(clist);
        printf("clients_tostring_test:\n%s", str);
        free(str);
    }
    return clist;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage : %s port\n", argv[0]);
        return EXIT_FAILURE;
    }
    start_server(atoi(argv[1]));

    // DB_save("db.txt", clients_tostring_test(false));
    //  testDB_save("dbloaded.txt", Users);
    return EXIT_SUCCESS;
}
