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

        int resultSelect = select(maxFD + 1, &readfds, NULL, NULL, NULL);
        check_error(resultSelect, "error in select()\n");

        if (FD_ISSET(server_socket, &readfds))
        {
            printf("New client connection\n");
            struct sockaddr_in csin = {0};
            size_t sinsize = sizeof csin;
            SOCKET csock = accept(server_socket, (struct sockaddr *)&csin, &sinsize);
            check_error(csock, "error in accept()\n");

            printf("New client connected from %s:%d - socket %d\n", inet_ntoa(csin.sin_addr),
                   htons(csin.sin_port), csock);

            if (csock > maxFD)
            {
                maxFD = csock;
            }
            FD_SET(csock, &readfds);
        }
        else
        {
            handle_request(readfds, maxFD);
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

void handle_request(fd_set readfds, int server_socket)
{
    char buffer[BUFFER_SIZE];
    int resultRecv = recv(server_socket, buffer, BUFFER_SIZE, 0);
    check_error(resultRecv, "error in recv()\n");

    switch (buffer[0])
    {
    case 0:
        printf("SUBSCRIBE request\n");
        handle_subscribe(buffer, server_socket);
        break;
    case 1:
        printf("UNSUBSCRIBE request\n");
        handle_unsubscribe(buffer, server_socket);
        break;
    case 2:
        printf("PUBLISH request\n");
        handle_publish(buffer, server_socket);
        break;
    case 3:
        printf("LIST request\n");
        handle_list(buffer, server_socket);
        break;
    case 4:
        printf("QUIT request\n");
        handle_quit(buffer, server_socket);
        break;
    case 5:
        printf("NEW ACCOUNT request\n");
        handle_new_account(buffer, server_socket);
        break;
    case 6:
        printf("LOGIN request\n");
        handle_login(buffer, server_socket);
        break;
    default:
        printf("Unknown request\n");
        break;
    }
}
void kill_server(SOCKET server_socket)
{
    close(server_socket);
}
void handle_subscribe(char *buffer, int client_socket)
{
}
void handle_unsubscribe(char *buffer, int client_socket)
{
}
void handle_publish(char *buffer, int client_socket)
{
}
void handle_list(char *buffer, int client_socket)
{
}
void handle_quit(char *buffer, int client_socket)
{
}
void handle_new_account(char *buffer, int client_socket)
{
}
void handle_login(char *buffer, int client_socket)
{
}

clientList *DB_load(char *file_path)
{

    FILE *file = fopen(file_path, "rb+");
    if (file == NULL)
    {
        printf("Error opening file\n");
        return NULL;
    }

    clientList *all_users = init_clientList(NULL);

    int users_number;
    fscanf(file, "%d", &users_number);
    // printf("users_number: %d\n", users_number);

    for (int i = 0; i < users_number; i++)
    {
        // Récuperer le pseudo
        char pseudo[PSEUDO_SIZE];
        fscanf(file, "%s", pseudo);
        // printf("pseudo %d: %s\n", i, pseudo);

        // Récuperer les abonnements
        int nSubbedTo;
        fscanf(file, "%d", &nSubbedTo);
        // printf("nSubbedTo : %d \n", nSubbedTo);

        clientList *subbed_to = init_clientList(NULL);
        for (int j = 0; j < nSubbedTo; j++)
        {
            char subbed_to_pseudo[PSEUDO_SIZE];
            fscanf(file, "%s", subbed_to_pseudo);
            add_client(subbed_to, init_client(0, subbed_to_pseudo, NULL, NULL));
            //   printf("%d est subbed to : %s\n", j, subbed_to_pseudo);
        }

        // Récupérer le nombre de messages
        int nMessages;
        fscanf(file, "%d", &nMessages);
        // printf("Nombre de messages : %d\n", nMessages);

        // Pour chaque message
        messageList *mlist = init_messageList(NULL, NULL);
        for (int j = 0; j < nMessages; j++)
        {
            // Récuperer le message
            char publication[PUBLICATION_SIZE];
            fscanf(file, "%s", publication);
            // printf("publication %d: %s\n", j, publication);

            // Récuperer la liste des receivers
            int nReceivers;
            fscanf(file, "%d", &nReceivers);
            // printf("nReceivers : %d \n", nReceivers);

            clientList *receivers = init_clientList(NULL);
            for (int k = 0; k < nReceivers; k++)
            {
                char receiver_pseudo[PSEUDO_SIZE];
                fscanf(file, "%s", receiver_pseudo);
                add_client(receivers, init_client(0, receiver_pseudo, NULL, NULL));
            }
            add_message(mlist, publication, receivers);
        }
        add_client(all_users, init_client(0, pseudo, mlist, subbed_to));
    }

    fclose(file);
    return all_users;
}

void DB_save(char *file_path, clientList *Users)
{
    FILE *file = fopen(file_path, "wb+");
    if (file == NULL)
    {
        printf("Error opening file");
    }

    fprintf(file, "%d\n", clientList_length(Users));
    char *data = clients_to_string(Users);
    fprintf(file, "%s", data);
    fclose(file);
    free(data);
}

void testDB_save(char *fpath, clientList *Users)
{
    char *data;
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

        data = clients_to_string(allUsers);
    }
    else
    {
        data = clients_to_string(Users);
    }
    FILE *file = fopen(fpath, "w+");
    fprintf(file, "%s", data);
    fclose(file);
}

clientList *testDB_load(char *fpath)
{
    return NULL;
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
    // if (argc != 2)
    // {
    //     printf("Usage : %s port\n", argv[0]);
    //     return EXIT_FAILURE;
    // }
    // start_server(atoi(argv[1]));

    // DB_save("db.txt", clients_tostring_test(false));

    clientList *clist = DB_load("../db.txt");
    if (clist == NULL)
    {
        printf("Couldnt load file. Quitting...\n");
        return EXIT_FAILURE;
    }
    DB_save("dbLOADED.txt", clist);
    return EXIT_SUCCESS;
}
