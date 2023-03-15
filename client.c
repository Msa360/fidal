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
        if (strcmp(argv[1], "ls") == 0) // todo: check if argv[1] exists else segmentation error
        {
            send(network_socket, "x01", sizeof("x01"), 0);
        } 
        else if (strcmp(argv[1], "get") == 0)
        {
            send(network_socket, "x02", sizeof("x02"), 0);
        }
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
    char *filename = "test.file2.txt";
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
