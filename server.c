// server.c

/*  architecture details
    example of a client request: "2main.c"
    The first byte (2 in this case) means the method, and the rest is the filename
    If method is 1, then every other byte after it should be ignore
*/

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
void list_dir_encoding(char* path, char* directories_mem, unsigned short mem_size);
int check_safe_path(char* path);


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

            char client_arg[51]; // 1 byte for arg and 50 bytes for filename including '\0'
            if (recv(client_socket, &client_arg, sizeof(client_arg), 0) <= 0) 
            {
                perror("[-]Bad client arg");
                close(client_socket);
                break;
            }
            if (client_arg[0] == 1) // ls request
            {
                char path[52];
                if (client_arg[1] == 0) {
                    memcpy(path, "./", 3);
                } else
                {
                    memcpy(path, "./", 2);
                    memcpy(path+2, client_arg+1, 49);
                    path[51] = '\0'; // appending null byte in case it wasn't included
                }
                printf("Client requested ls %s\n", path);
                unsigned short mem_size = 750;
                char* dir_memory = malloc(mem_size);
                list_dir_encoding(path, dir_memory, mem_size);    // formats the filenames to be send
                send(client_socket, dir_memory, mem_size, 0);
                free(dir_memory);
    
                close(client_socket);
                break;
            }
            else if (client_arg[0] == 2) // get request
            {
                printf("Client requested: %s\n", client_arg + 1); // client_arg + 1 to skip method byte
                
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

void list_dir_encoding(char* path, char* directories_mem, unsigned short mem_size)
{
    if (check_safe_path(path) == 0) {
        *directories_mem = 2;         // 2 for invalid path error code
        *(directories_mem + 1) = 0;   // then terminating with null byte
        return;
    }

    DIR *dp;
    struct dirent *ep;
    dp = opendir(path);  // todo: check if user isn't reaching higher directories

    if (dp != NULL)
    {   
        int position = 1;   // start at 1 to let place for succes/error byte
        while ((ep = readdir(dp)) != NULL)
        {
            unsigned char name_length = (unsigned char) strlen(ep->d_name);
            if (position + 2 + name_length < mem_size)   // to not exceed the allocated memory
            {
                if (strcmp(ep->d_name, ".") != 0 && strcmp(ep->d_name, "..") != 0) {
                    if (ep->d_type == DT_REG || ep->d_type == DT_UNKNOWN)
                    {
                        *(directories_mem + position) = (char) 1;
                    } else if (ep->d_type == DT_DIR)
                    {
                        *(directories_mem + position) = (char) 2;
                    }
                    position++;

                    *(directories_mem + position) = name_length;
                    position++;
                    memcpy(directories_mem + position, ep->d_name, name_length);
                    position += name_length;
                }
            }
        }
        *directories_mem = 1;   // start of sequence success byte
        *(directories_mem + position) = '\0';   // end of sequence

        closedir(dp);
    }
    else
    {
        perror("Couldn't open the directory");
        *directories_mem = 0;         // 0 for error
        *(directories_mem + 1) = 0;   // then terminating with null byte
    }
}

int check_safe_path(char* path) {
    // a safe path means the path doesn't go in higher directories ex: ../
    // return 0 if unsafe, 1 if safe
    char last_tok = -1;
    unsigned int tok_index = 0; 
    while (last_tok != '\0')
    {   
        if (path[tok_index] == '.' && last_tok == '.') {
            return 0;
        }
        last_tok = path[tok_index];
        tok_index++;
    }
    return 1;
}