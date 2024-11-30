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


    // Login
    char message[1024];
    sprintf(message, "USER %s\r\n", c.username);
    
    if(compute_response(message, NULL, 0) < 0){
        printf("Error sending USER command\n");
        return -1;
    }


    // Fechar a conexão
    if (close(sockfd) < 0) {
        perror("close()");
        exit(-1);
    }

    return 0;
}
