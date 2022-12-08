#include "client.h"

#define check_error(expr, userMsg) \
    do                             \
    {                              \
        if (expr == -1)            \
        {                          \
            perror(userMsg);       \
            exit(errno);           \
        }                          \
    } while (0)

int request_server(SOCKET client_socket, char *buffer)
{
    printf("Enter your request: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strlen(buffer) - 1] = '\0';

    int resultSend = send(client_socket, buffer, strlen(buffer), 0);
    check_error(resultSend, "error in send()\n");

    return resultSend;
}

int receive_server(SOCKET client_socket, char *buffer)
{
    int resultRecv = recv(client_socket, buffer, BUFFER_SIZE, 0);
    check_error(resultRecv, "error in recv()\n");

    if (resultRecv == 0)
    {
        printf("Server closed connection\n");
        return 0;
    }

    printf("Server response: %s\n", buffer);
    return 1;
}

void start_client(char *ip, int port)
{
    SOCKET socket_client = init_client(ip, port);
    char buffer[BUFFER_SIZE];

    fd_set readfds;
    int maxFD = socket_client;

    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(socket_client, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        int resultSelect = select(maxFD + 1, &readfds, NULL, NULL, NULL);
        check_error(resultSelect, "error in select()\n");

        if (FD_ISSET(STDIN_FILENO, &readfds))
        {
            if (!request_server(socket_client, buffer))
            {
                break;
            }
        }
        else if (FD_ISSET(socket_client, &readfds))
        {
            if (!receive_server(socket_client, buffer))
            {
                break;
            }
        }
    }
    kill_client(socket_client);
}

void kill_client(SOCKET client_socket)
{
    printf("Closing client socket\n");
    close(client_socket);
}

int init_client(char *ip, int port)
{
    SOCKET socketClient = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in sin;
    socklen_t taille = sizeof(struct sockaddr);

    check_error(socketClient, "error in initialization socket()\n");

    sin.sin_addr.s_addr = inet_addr(ip);
    sin.sin_port = htons(port);
    sin.sin_family = AF_INET;
    int connectionResult = connect(socketClient, (struct sockaddr *)&sin, taille);
    check_error(connectionResult, "error in connect()\n");

    return socketClient;
}

void run()
{
    // Login or create account
    // If login, print all published messages
    // If create account, print success
    // Ask for user input
    // Print server response
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Usage: %s <ip> <port> \n", argv[0]);
        return EXIT_FAILURE;
    }
    printf("Welcome on Cwitter -----------\n\n\n");
    start_client(argv[1], atoi(argv[2]));
    run();

    return EXIT_SUCCESS;
}