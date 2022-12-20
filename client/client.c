#include "client.h"

#define check_error(expr, userMsg) \
    do                             \
    {                              \
        if (expr < 0)            \
        {                          \
            perror(userMsg);       \
            exit(errno);           \
        }                          \
    } while (0)


void handle_list(SOCKET client_socket, char *buffer){
    printf("\nList connected users\n");
    //on met 1 en entete du buffer
    buffer[0] = '1';    
    write_to_server(client_socket, buffer);
}

void handle_subscribe(SOCKET client_socket, char *buffer){

}

void handle_unsubscribe(SOCKET client_socket, char *buffer){

}

void handle_publish(SOCKET client_socket, char *buffer){

}

int request_server(SOCKET client_socket, char *buffer)
{

    
    int c;   
    static struct termios oldt, newt;

    
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);          
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
    while((c=getchar())!= EOF){
        if(c == '1'){
            //List connecteds users
            handle_list(client_socket, buffer);
            return 1;
        }
        else if(c == '2'){
            //Subscribe to user
        }
        else if(c == '3'){
            //unsubscribe to user
        }
        else if(c == '4'){
            //Publish message
        }
        else if(c == '5'){
            //Quit
        }
        else{
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
    int resultRecv = recv(client_socket, buffer, BUFFER_SIZE, 0);
    check_error(resultRecv, "error in recv()\n");
    printf("Buffer : %s\n", buffer);

    if (resultRecv == 0)
    {
        printf("Server closed connection\n");
        return 0;
    }

    printf("Server response: %s\n", buffer);
    return 1;
}

void start_client(char *ip, int port,char* pseudo)
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
            //print_menu();
        }

        FD_SET(socket_client, &readfds);
        FD_SET(STDIN_FILENO, &readfds);

        int resultSelect = select(maxFD + 1, &readfds, NULL, NULL, NULL);
        check_error(resultSelect, "error in select()\n");

        print=1;

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


char* auth()
{
    printf("1. Login\n");
    printf("2. Create account\n");
    printf("3. Quit\n\n");
    
    int c;   
    static struct termios oldt, newt;

    
    tcgetattr( STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON);          
    tcsetattr( STDIN_FILENO, TCSANOW, &newt);
    while((c=getchar())!= EOF){
        if(c == '1'){
            printf("\nLogin\n");
            while(1){
                char* pseudo = malloc(20);
                char* name = malloc(20);
                pseudo[0] = '7';
                printf("Enter your pseudo: ");
                fgets(name, 20, stdin);
                if (strlen(name) >6){
                    printf("Pseudo too long\n");
                    free(name);
                    continue;
                }
                else {
                    pseudo = strcat(pseudo, name);
                    printf("Pseudo: %s\n", name);
                    return pseudo;
                }
            }       
        }
        else if(c == '2'){
            printf("\nCreate account\n");
            while(1){
                char* pseudo = malloc(20);
                char* name = malloc(20);
                pseudo[0] = '6';
                printf("Enter your pseudo: ");
                fgets(name, 20, stdin);
                if (strlen(name) >6){
                    printf("Pseudo too long\n");
                    free(name);
                    continue;
                }
                else {
                    pseudo = strcat(pseudo, name);
                    printf("Pseudo: %s\n", pseudo);
                    char* buffer = malloc(30);
                    return pseudo;
                }
            }
        }
        else if(c == '3'){
            printf("\nQuit\n");
            exit(0);
        }
        else{
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

void print_menu(){
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
    char* pseudo = auth();
    start_client(argv[1], atoi(argv[2]),pseudo);

    return EXIT_SUCCESS;
}