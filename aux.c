#include "aux.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

int sockfd;
//código reutilizado do exemplo fornecido
char* get_ip(char* hostname) {
    struct hostent *h;
    if ((h = gethostbyname(hostname)) == NULL) {
        herror("gethostbyname()");
        exit(-1);
    }
    return inet_ntoa(*((struct in_addr *) h->h_addr));
}
int create_socket(const char* ip, int port){
    struct sockaddr_in server_addr;
    char buf[1024]; // Buffer para armazenar a resposta do servidor
    ssize_t bytes;

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
    printf("Connection estabilished\n");
    bytes = read(sockfd, buf, sizeof(buf) - 1);
    if (bytes > 0) {
        buf[bytes] = '\0'; // Garantir terminação da string
        printf("\n%s", buf);
    } else {
        perror("read()");
    }

    return sockfd;
}

int send_socket(char* message){
    size_t bytes;
    bytes = write(sockfd, message, strlen(message));
    if (bytes < 0){
        perror("write()");
        exit(-1);
    }
    return 0;
}

int close_socket(int* sockfd){
    if (close(sockfd)<0) {
        perror("close()");
        exit(-1);
    }
    return 0;
}

//exemplo de url= ftp://[<user>:<password>@]<host>/<url-path>

int parse_url(struct components *c, char* url){
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
        strcpy(c->username, "anonymous");
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
        strcpy(c->password, "");
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

int compute_response(char* message, char* response, int response_size){
    char buf[1024]; // Buffer para armazenar a resposta do servidor
    ssize_t bytes;
    
    if(send_socket(message) < 0){
        return -1;
    }
    
    bytes = read(sockfd, buf, sizeof(buf) - 1);
    if (bytes > 0) {
        buf[bytes] = '\0'; // Garantir terminação da string
        printf("\n%s", buf);
    } else {
        perror("read()");
    }
    response = buf[0];
    printf("The response received is: %s\n", response);
    // printf("Sent message: %s\n", message);

    // memset(response, 0, response_size);
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

int parse_pasv_response(const char* response, char* ip, int* port) {
    int x1, x2, x3, x4, p1, p2;
    char* start = strchr(response, '(');
    char* end = strchr(response, ')');

    if (!start || !end || end < start) {
        printf("Invalid PASV response: %s\n", response);
        return -1;
    }

    // Extraindo números no formato (x1,x2,x3,x4,p1,p2)
    if (sscanf(start, "(%d,%d,%d,%d,%d,%d)", &x1, &x2, &x3, &x4, &p1, &p2) != 6) {
        printf("Failed to parse PASV response: %s\n", response);
        return -1;
    }

    // Construindo o IP e calculando a porta
    *port = p1 * 256 + p2;
    return 0;
}