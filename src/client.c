// client.c

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h> // for memset

#define SIZE 1024 // size of chuncks of data


void receive_file(int sockfd);

int main(int argc, char const **argv)
{
    char* ip = "127.0.0.1";
    int port = 3200;

    int network_socket;
    network_socket = socket(AF_INET, SOCK_STREAM, 0);

    // define the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = inet_addr(ip);
    
    int connection_status = connect(network_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (connection_status == -1)
    {
        printf("[-]Client error while trying to connect with server\n");
        exit(1);
    }
    // char server_resp[256];

    // int i = 0;  
    // while (i < 5)
    // {
    //     if(recv(network_socket, &server_resp, sizeof(server_resp), 0))
    //     {
    //         printf("Server data is: %s\n", server_resp);
    //     }
    //     i++;
    // }

    if (argc >= 2)
    {
        if (strcmp(argv[1], "ls") == 0)
        {
            char message[51]; // means ls
            message[0] = 1;
            if (argc >= 3)
            {
                memcpy(message+1, argv[2], 50);
                message[50] = '\0';
            }
            else { message[1] = '\0'; }

            send(network_socket, &message, sizeof(message), 0);    // send ls request

            char filenames[SIZE];   // bytes reserved to store filenames
            if (recv(network_socket, filenames, sizeof(filenames), 0) <= 0) {
                printf("An error happened with the server\n");
            }
            if (filenames[0] == 1)  // success
            {
                int position = 1;   // keeps track of where we are in the message
                char status = 1;    // to stop while loop
                while (status) {
                    switch (filenames[position]) {
                    case '\0':
                        printf("\n");
                        status = 0;
                        break;
                    case 1:     // normal file
                        position++;
                        printf("%.*s\n", filenames[position], filenames + position + 1);
                        position += filenames[position] + 1;
                        break;
                    case 2:     // directory
                        position++;
                        printf("\033[1;32m%.*s\033[0m\n", filenames[position], filenames + position + 1); // prints in green
                        position += filenames[position] + 1;
                        break;

                    default:
                        printf("An error happened in the server response\n");
                        break;
                    }
                }
            } else if (filenames[0] == 2) {  // unsafe path
                printf("Invalid path, cannot go in higher directories\n");
            } else {
                printf("An error happened in the server response\n");
            }
            close(network_socket);
            return 0;
            
        }
        else if (strcmp(argv[1], "get") == 0)
        {
            if (argc >= 3) {
                char method = 2; // means get 
                char request_message[51];
                request_message[0] = method;
                request_message[1] = 0;
                strcat(request_message, argv[2]); // todo: change to argv[3] after ip:port is implemented
                send(network_socket, request_message, sizeof(request_message), 0);
            } else
            {
                printf("Error: no file given in argument\n");
                close(network_socket);
                exit(0);
            }
            
        }
    } else {
        printf("Error: not enough arguments\n");
        close(network_socket);
        exit(0);
    }
    
    receive_file(network_socket);
    printf("[+]File received!\n");
 
    close(network_socket);
    printf("[+]Disconnected from the server.\n");

    return 0;
}


void receive_file(int sockfd)
{
    int n; 
    FILE *fp;
    char *filename = "test.received.txt";  // todo: change to name it the same name as original file
    char buffer[SIZE];

    fp = fopen(filename, "w");
    if (fp == NULL)
    {
        perror("[-]Error in creating file\n");
        exit(1);
    }

    while (1)
    {
        n = recv(sockfd, buffer, SIZE, 0);
        if (n <= 0)
        {
            break;
            return;
        }
        fprintf(fp, "%s", buffer);
        memset(buffer, '\0', SIZE);
    }
}
