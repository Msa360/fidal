// server.c

#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h> // for memset
#include <dirent.h> // for listdir

#define SIZE 1024 // size of chuncks of data


void send_file(FILE *fp, int sockfd);


int main(int argc, char const **argv)
{

    // char* ip = "127.0.0.1"; // now using INADDR_ANY
    unsigned int port = 3200;   

    int server_socket = socket(AF_INET, SOCK_STREAM, 0);

    // define the server address
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    server_address.sin_addr.s_addr = INADDR_ANY; //inet_addr(ip);

    if (bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
    {
        perror("[-]Server error while binding socket\n");
        exit(1);
    }

    listen(server_socket, 2);

    int client_socket = accept(server_socket, NULL, NULL);


    char client_arg[256];

    int n = recv(client_socket, &client_arg, sizeof(client_arg), 0);
    printf("Client arg is: %s\n", client_arg);



    FILE *fileP = fopen("test.txt", "r");
    if (fileP == NULL)
    {
        perror("[-]Server error while opening file\n");
        exit(1);
    }
    

    send_file(fileP, client_socket);
    printf("[+]File data sent successfully.\n");
    fclose(fileP);
    if (close(server_socket) == 0)
    {
        printf("[+]Disconnected the server.\n");
    } else
    {
        printf("[-]Error while disconnecting");
    }
    
    return 0;
}



void send_file(FILE *fp, int sockfd)
{
    int n;
    char data[SIZE] = {0};
 
    while(fgets(data, SIZE, fp) != NULL) {
        if (send(sockfd, data, sizeof(data), 0) == -1) {
            perror("[-]Error in sending file.");
            exit(1);
        }
        memset(data, '\0', SIZE);
    }
}

void list_dirs()
{
    DIR *dp;
    struct dirent *ep;     
    dp = opendir("./");

    // initialize directories list
    char** directories = malloc(5*sizeof(char*));
    for(int i = 0; i < 5; i++){
        directories[i] = (char*)malloc(50*sizeof(char));
    }

    int position = 0;
    if (dp != NULL)
    {   
        while ((ep = readdir(dp)) != NULL)
        {
            directories[position] = ep->d_name;
            position++;
        }
        (void) closedir(dp);

        int index = 0;
        while (1)
        {
            if (directories[index] != NULL)
            {
                printf("%s ", directories[index]);
                index++;
            }
            else
            {
                printf("\n");
                break;
            }
        }
    }
    else
    {
        perror("Couldn't open the directory");
        exit(1);
    }
}
