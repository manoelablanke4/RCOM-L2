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

    //Change dir
    if(c.path[0] != '\0')
    {   
        if(send_socket(c.path, "CWD") < 0){
            printf("Error changing directory\n");
            return -1;
        }
        if(read_socket(response, MAX_LENGTH) < 0){
            printf("Error reading socket\n");
            return -1;
        }
        if(response[0] != '2'){
            printf("Error changing directory\n");
            return -1;
        }
    }
    int port;
    char ip[MAX_LENGTH];

    //Passive mode
    if(send_socket("", "PASV") < 0){
        printf("Error entering passive mode\n");
        return -1;
    }
    if(read_socket(response, MAX_LENGTH) < 0){
        printf("Error reading socket\n");
        return -1;
    }
    if(response[0] != '2'){
        printf("Error entering passive mode\n");
        return -1;
    }
    if(parse_pasv_response(response, ip, port) < 0){
        printf("Error parsing PASV response\n");
        return -1;
    }

    if(create_socket(ip, port) < 0){
        printf("Error creating socket\n");
        return -1;
    }

    //Retrieving file
    if(send_socket(c.filename, "RETR") < 0){
        printf("Error retrieving file\n");
        return -1;
    }
    if(read_socket(response, MAX_LENGTH) < 0){
        printf("Error reading socket\n");
        return -1;
    }
    if(response[0] != '2'){
        printf("Error retrieving file\n");
        return -1;
    }
    // Fechar a conexão
    if(close_socket() < 0){
        printf("Error closing socket\n");
        return -1;
    }

    return 0;
}
