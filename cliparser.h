#pragma once

void parse(char **argv, char *usage_type, char *ip, unsigned int *port, char *filename)
{
    if (argv[1] == "listen")
    {
        usage_type = "server";
        // todo: check if port argv[3] is a valid port int
        *port = argv[2];
        
        
    } 
    else if (argv[1] == "get")
    {
        *usage_type = "client";
        split_ip_port(argv[2], ip, port);
        filename = argv[3];
    }
    else
    {
        printf("Invalid usage!\n");
        printf("Usage: fidal listen [port]");
        printf("Usage: fidal get [ip:port] [filename]");
        printf("Usage: fidal ls [ip:port]"); // list the available files on the server
        exit(1);
    }
    
}

void split_ip_port(char* str, char* ip, unsigned int* port) // inplace change
{
    int index = 0;
    for (size_t i = 0; i < strlen(str); i++)
    {
        if (str[i] == ':')  
        {
            index = i;
            char temp_port[30];
            memcpy(ip, str, index); // for the ip
            memset(&ip[index], '\0', 1); // add end sequence
            memcpy(temp_port, &str[index+1], strlen(str)-index+1); // for the port
            memset(&temp_port[strlen(str)-index+1], '\0', 1); // add end sequence

            char *cache; // this catches the text part after the numerical values of str (useless in this case)
            *port = (unsigned int) strtol(temp_port, &cache, 10);
            break;
        }
    }
}