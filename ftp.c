#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "aux.h"

#define SERVER_PORT 21 // Porta padrão do FTP

extern int sockfd;

int main(int argc, char* argv[]) {
    if (argc != 2) {
    printf("Usage: %s <ftp-url>\n", argv[0]);
    return -1;
    }
    
    char response[MAX_LENGTH];
    // char buf[1024];
    struct components c;

    if(parse_url(&c, argv[1]) < 0){
        printf("Error parsing URL\n");
        return -1;
    }

    char* host_ip = get_ip(c.hostname);
    int sockfd;

    if(create_socket(host_ip, SERVER_PORT) < 0){
        printf("Error creating socket\n");
        return -1;
    }

    if(read_socket(response, MAX_LENGTH) < 0){
        printf("Error reading socket\n");
        return -1;
    }
    if (response[0] != '2')
    {
        printf("Error connecting to server\n");
        return -1;
    }
    // Login
    if(login(c, response) < 0){
        printf("Error logging in\n");
        return -1;
    }
    if(response[0] != '2'){
        printf("Error logging in\n");
        return -1;
    }

    printf("The path is %s\n", c.path);
    printf("The file is %s\n", c.filename);

    // Fechar a conexão
    if (close_socket() < 0) {
        perror("close()");
        exit(-1);
    }

    return 0;
}
