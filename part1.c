#include "part1.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>


//código reutilizado do exemplo fornecido
char* get_ip(char* hostname) {
    struct hostent *h;
    if ((h = gethostbyname(hostname)) == NULL) {
        herror("gethostbyname()");
        exit(-1);
    }
    return inet_ntoa(*((struct in_addr *) h->h_addr));
}

int create_socket(const char* ip, int port ){
    int sockfd;
    struct sockaddr_in server_addr;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket()");
        exit(-1);
    }

    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(ip);
    server_addr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("connect()");
        exit(-1);
    }

    return sockfd;
}

int send_socket(int sockfd, char* message){
    size_t bytes;
    bytes = write(sockfd, message, strlen(message));
    if
    if (bytes > 0)
        printf("Written Bytes %ld\n", bytes);
    else {
        perror("write()");
        exit(-1);
    }
    return 0;
}

int close_socket(int sockfd){
    if (close(sockfd)<0) {
        perror("close()");
        exit(-1);
    }
    return 0;
}

//exemplo de url= ftp://[<user>:<password>@]<host>/<url-path>

int parse_url(components* c, char* url){
    char* check = strstr(url, "ftp://");
    if(check == NULL){
        printf("Invalid URL: doesn't contain ftp://\n");
        return -1;
    }
    check += 6;
    char* user = strstr(check, ":");
    if(user == NULL){
        printf("Invalid URL: missing ':' for username\n");
        return -1;
    }
    strncpy(c->username, check, user-check);
    if(strlen(c->username) == 0){
        c->username = "anonymous";
    }
    c->username[user-check] = '\0';
    check = user+1;
    char* pass = strstr(check, "@");
    if(pass == NULL){
        printf("Invalid URL: missing '@' for password\n");
        return -1;
    }
    strncpy(c->password, check, pass-check);
    if(strlen(c->password) == 0){
        c->password = "";
    }
    c->password[pass-check] = '\0';
    check = pass+1;
    char* host = strstr(check, "/");
    if(host == NULL){
        printf("Invalid URL: missing '/' for host\n");
        return -1;
    }
    strncpy(c->hostname, check, host-check);
    c->hostname[host-check] = '\0';
    check = pass + 1;
    char* url_path = strstr(check, "/");
    if(url_path == NULL){
        printf("Invalid URL: missing '/' for path\n");
        return -1;
    }
    strncpy(c->path, url_path, url_path-check);
    c->path[url_path-check] = '\0';
    
    char* file_name = strrchr(url_path, '/');
    if(file_name == NULL){
        printf("Invalid URL: missing '/' for filename\n");
        return -1;
    }
    strcpy(c->filename, file_name+1);
    return 0;
}

int compute_response(int sockfd, char* message, char* response, int response_size){
    if(send_socket(sockfd, message) < 0){
        return -1;
    }
    printf("Sent message: %s\n", message);

    memset(response, 0, response_size);
    FILE* fp = fdopen(sockfd, "r");

    if(fp == NULL){
        perror("fdopen()");
        return -1;
    }

   while (!(response[0] >= '1' && response[0] <= '5') || response[3] != ' '){
        if(read(response, response_size, fp) == NULL){
            perror("read()");
            return -1;
        }
    }

    int first_digit = response[0] - '0';

    switch (first_digit){
        case 1:
            printf("Received preliminary reply (code 1xx): %s\n", response);
            break;
        case 2:
            printf("Received positive completion reply (code 2xx): %s\n", response);
            break;
        case 3:
            printf("Received positive intermediate reply (code 3xx): %s\n", response);
            break;
        case 4:
            printf("Received transient negative completion reply (code 4xx): %s\n", response);
            break;
        case 5:
            printf("Received permanent negative completion reply (code 5xx): %s\n", response);
            break;
        default:
            printf("Received unknown reply: %s\n", response);
            break;
    }

    return 0;

   }

