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

    while (1)
    {
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);

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
            handle_client(readfds, maxFD);
        }
    }
    end_server(server_socket);
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

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Usage : %s port\n", argv[0]);
        return EXIT_FAILURE;
    }
    start_server(atoi(argv[1]));

    return EXIT_SUCCESS;
}
