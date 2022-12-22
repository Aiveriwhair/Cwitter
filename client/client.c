#include "client.h"

#define check_error(expr, userMsg) \
    do                             \
    {                              \
        if (expr < 0)              \
        {                          \
            perror(userMsg);       \
            exit(errno);           \
        }                          \
    } while (0)

void handle_list(SOCKET client_socket, char *buffer)
{
    buffer[0] = '1';
    write_to_server(client_socket, buffer);
}

void handle_subscribe(SOCKET client_socket, char *buffer)
{
    buffer[0] = '2';
    printf("-Enter the username of the user you want to subscribe to: ");
    fgets(buffer + 1, BUFFER_SIZE, stdin);
    buffer[strlen(buffer) - 1] = '\0';
    write_to_server(client_socket, buffer);
}

void handle_unsubscribe(SOCKET client_socket, char *buffer)
{
    buffer[0] = '3';
    printf("-Enter the username of the user you want to unsubscribe to: ");
    fgets(buffer + 1, BUFFER_SIZE, stdin);
    buffer[strlen(buffer) - 1] = '\0';
    write_to_server(client_socket, buffer);
}

void handle_publish(SOCKET client_socket, char *buffer)
{
}

int request_server(SOCKET client_socket, char *buffer)
{

    int c;
    static struct termios oldt, newt;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    while ((c = getchar()) != EOF)
    {
        if (c == '1')
        {
            // List connecteds users
            handle_list(client_socket, buffer);
            return 1;
        }
        else if (c == '2')
        {
            // Subscribe to user
            handle_subscribe(client_socket, buffer);
            return 1;
        }
        else if (c == '3')
        {
            // unsubscribe to user
            handle_unsubscribe(client_socket, buffer);
            return 1;
        }
        else if (c == '4')
        {
            // Publish message
        }
        else if (c == '5')
        {
            // Quit
            kill_client(client_socket);
        }
        else
        {
            printf("Wrong input\n");
        }
    }

    // fgets(buffer, BUFFER_SIZE, stdin);
    // buffer[strlen(buffer) - 1] = '\0';

    // int resultSend = send(client_socket, buffer, strlen(buffer), 0);
    // check_error(resultSend, "error in send()\n");

    // return resultSend;
    return 0;
}

int receive_server(SOCKET client_socket, char *buffer)
{
    memset(buffer, '\0', BUFFER_SIZE);
    int resultRecv = recv(client_socket, buffer, BUFFER_SIZE, 0);
    check_error(resultRecv, "error in recv()\n");

    if (resultRecv == 0)
    {
        printf("Server closed connection\n");
        return 0;
    }

    switch (buffer[0])
    {
    case '1':
        printf("-This is the list of users of Cwitter:\n");
        for (int i = 1; i < strlen(buffer); i++)
        {
            if (buffer[i] == '-')
            {
                printf("\n");
            }
            else
            {
                printf("%c", buffer[i]);
            }
        }
        printf("\n");
        break;
    case '2':
        printf("\nYou have subscribed to %s\n\n", buffer + 1);
        break;
    case '3':
        printf("\nYou have unsubscribed from %s\n", buffer + 1);
        break;
    case '4':
        printf("PUBLISH request\n");
        break;
    case '5':
        printf("QUIT request\n");
        break;
    case 'e':
        handle_error(buffer);
        break;
    default:
        printf("Unknown request\n");
        break;
    }
    return 1;
}

void handle_error(char *buffer)
{
    switch (buffer[1])
    {
    case 's':
        printf("\n ERROR : You can't subscribe to yourself ! \n");
        break;
    case 'n':
        printf("\n ERROR : This user doesn't exist ! \n");
        break;
    case 'l':
        printf("\n ERROR : Login error, account doesn't exist\n");
        // Return to authentification
    case 'd':
        printf("\n ERROR : You have already subscribe to this user ! \n");
        break;
    case 'u':
        printf("\n ERROR : You can't unsubscrbe to this user because you don't follow him ! \n");
        break;
    case 't':
        printf("\n ERROR : You can't unsubscribe to yourself ! \n");
        break;
    case 'r':
        printf("\n ERROR : You haven't follow someone yet ! \n");
        break;
    default:
        break;
    }
}

void start_client(char *ip, int port, char *pseudo)
{
    SOCKET socket_client = init_client(ip, port);
    char buffer[BUFFER_SIZE];

    fd_set readfds;
    int maxFD = socket_client;

    write_to_server(socket_client, pseudo);
    printf("Authentification successful \n");
    print_menu();
    int print = 0;
    FD_ZERO(&readfds);
    while (1)
    {

        if (print)
        {
            // print_menu();
        }

        FD_SET(socket_client, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        int resultSelect = select(maxFD + 1, &readfds, NULL, NULL, NULL);
        check_error(resultSelect, "error in select()\n");

        print = 1;

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

char *auth()
{
    printf("1. Login\n");
    printf("2. Create account\n");
    printf("3. Quit\n\n");

    int c;
    static struct termios oldt, newt;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    while ((c = getchar()) != EOF)
    {
        if (c == '1')
        {
            printf("\nLogin\n");
            while (1)
            {
                char *pseudo = malloc(20);
                char *name = malloc(20);
                pseudo[0] = '7';
                printf("Enter your pseudo: ");
                fgets(name, 20, stdin);
                if (strlen(name) > 6)
                {
                    printf("Pseudo too long\n");
                    free(name);
                    continue;
                }
                else
                {
                    pseudo = strcat(pseudo, name);
                    printf("Pseudo: %s\n", name);
                    return pseudo;
                }
            }
        }
        else if (c == '2')
        {
            printf("\nCreate account\n");
            while (1)
            {
                char *pseudo = malloc(20);
                char *name = malloc(20);
                memset(pseudo, '\0', 20);
                memset(name, '\0', 20);
                pseudo[0] = '6';
                printf("Enter your pseudo: ");
                fgets(name, 20, stdin);
                if (strlen(name) > 6)
                {
                    printf("Pseudo too long\n");
                    free(name);
                    free(pseudo);
                    continue;
                }
                else
                {
                    pseudo = strcat(pseudo, name);
                    printf("Pseudo: %s\n", pseudo);
                    return pseudo;
                }
            }
        }
        else if (c == '3')
        {
            printf("\nQuit\n");
            exit(0);
        }
        else
        {
            printf("\nWrong input\n");
        }
    }
    return "ERROR WHILE AUTH";
}

void write_to_server(SOCKET client_socket, char *buffer)
{
    int resultSend = send(client_socket, buffer, strlen(buffer), 0);
    check_error(resultSend, "error in write to server send()\n");
}

void print_menu()
{
    printf("\n You can now use and navigate in Cwitter ! \n");
    printf("\n------------------------- Menu -------------------------\n\n");
    printf("1 ------ list all users of Cwitter, connected or not\n");
    printf("2 ------ subscribe to pseudo's feed\n");
    printf("3 ------ unsubscribe to pseudo's feed\n");
    printf("4 ------ publish a message on your feed\n");
    printf("5 ------ quit Cwitter\n\n\n");
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        printf("Usage: %s <ip> <port> \n", argv[0]);
        return EXIT_FAILURE;
    }
    printf("\n\n\n----------- Welcome on Cwitter -----------\n\n\n");
    char *pseudo = auth();
    pseudo[strlen(pseudo) - 1] = '\0';
    start_client(argv[1], atoi(argv[2]), pseudo);

    return EXIT_SUCCESS;
}