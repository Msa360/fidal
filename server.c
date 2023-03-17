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

    int server_socket;
    if ((server_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)  
    {  
        perror("socket failed");  
        exit(1);  
    }  
    //set socket to allow multiple connections, might delete
    int opt = 1; // TRUE
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0)  
    {  
        perror("setsockopt::SO_REUSEADDR");  
        exit(1);  
    }
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEPORT, (char *)&opt, sizeof(opt)) < 0)  
    {  
        perror("setsockopt::SO_REUSEPORT");  
        exit(1);  
    }

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

    if (listen(server_socket, 30) < 0)  
    {  
        perror("listen");  
        exit(1);  
    } 


    int client_socket;
    int childpid;

    while (1) {
    
        if ((client_socket = accept(server_socket, NULL, NULL)) < 0)
        {
            perror("accept");
            exit(1);
        }

        if ((childpid = fork()) == 0) {

            close(server_socket);

            char client_arg[42]; // 1 byte for arg and 40 bytes for filename, 1 byte for '\0'
            if (recv(client_socket, &client_arg, sizeof(client_arg), 0) <= 0) 
            {
                perror("[-]Bad client arg\n");
            }
            printf("Client arg is: %s\n", client_arg);

            if (client_arg[0] == 2) 
            {
                char *filename = client_arg + 1;

                FILE *fileP = fopen(filename, "r");
                if (fileP == NULL)
                {
                    perror("[-]Server error while trying to open file\n");
                    exit(1);
                }
                send_file(fileP, client_socket);
                printf("[+]File data sent successfully.\n");
                fclose(fileP);
                close(client_socket);
                break;

            } else {
                break;
            }

        } else {
            close(client_socket);
        }
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
