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
    memset(buf, 0, sizeof(buf));

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

    return sockfd;
}

int send_socket(char* message, char* header){
    size_t bytes;

    bytes = write(sockfd, header, strlen(header));
    if (bytes != strlen(header)) {
        perror("write()");
        exit(-1);
    }
    bytes = write(sockfd, " ", 1);
    if (bytes != 1) {
        perror("write()");
        exit(-1);
    }
    bytes = write(sockfd, message, strlen(message));
    if (bytes != strlen(message)) {
        perror("write()");
        exit(-1);
    }
    bytes = write(sockfd, "\n", 1);
    if (bytes != 1) {
        perror("write()");
        exit(-1);
    }
    printf("%s %s\n", header, message);

    return 0;
}

int close_socket(){
    if (close(sockfd)<0) {
        perror("close()");
        exit(-1);
    }
    return 0;
}

//exemplo de url= ftp://[<user>:<password>@]<host>/<url-path>

int parse_url(struct components *c, char* url) {
    char* check = strstr(url, "ftp://");
    if (check == NULL) {
        printf("Invalid URL: doesn't contain ftp://\n");
        return -1;
    }
    check += 6; // Avança para o conteúdo após "ftp://"

    // Verifica se há username e password ou pula direto para o hostname
    char* at = strchr(check, '@');
    if (at != NULL) {
        // Extrai username
        char* colon = strchr(check, ':');
        if (colon != NULL && colon < at) {
            strncpy(c->username, check, colon - check);
            c->username[colon - check] = '\0';

            check = colon + 1; // Avança para a senha
            strncpy(c->password, check, at - check);
            c->password[at - check] = '\0';
        } else {
            printf("Invalid URL: missing ':' for username/password\n");
            return -1;
        }
        check = at + 1; // Avança para o hostname
    } else {
        // Define valores padrão para username e password
        strcpy(c->username, "anonymous");
        strcpy(c->password, "anonymous");
    }

    // Extrai hostname
    char* slash = strchr(check, '/');
    if (slash == NULL) {
        printf("Invalid URL: missing '/' for path\n");
        return -1;
    }
    strncpy(c->hostname, check, slash - check);
    c->hostname[slash - check] = '\0';

    check = slash + 1; // Avança para o caminho

    // Extrai path e filename
    char* file_name = strrchr(check, '/');
    if (file_name == NULL) {
        printf("Invalid URL: missing '/' for filename\n");
        return -1;
    }
    strncpy(c->path, check, file_name - check);
    c->path[file_name - check] = '\0';

    strcpy(c->filename, file_name + 1);

    return 0;
}
int read_socket(char* response, size_t response_size){

    FILE* fp = fdopen(sockfd, "r");

    do {
        memset(response, 0, sizeof(response));
        response = fgets(response, response_size, fp);
        printf("%s", response);
    } while (!('1' <= response[0] && response[0] <= '5') || response[3] != ' ');

    // response = buf[0];    // printf("Sent message: %s\n", message);



    // if(fp == NULL){
    //     perror("fdopen()");
    //     return -1;
    // }

    // while (!(response[0] >= '1' && response[0] <= '5') || response[3] != ' '){
    //     if(read(response, response_size, fp) == NULL){
    //         perror("read()");
    //         return -1;
    //     }
    // }

    // int first_digit = response[0] - '0';

    // switch (first_digit){
    //     case 1:
    //         printf("Received preliminary reply (code 1xx): %s\n", response);
    //         break;
    //     case 2:
    //         printf("Received positive completion reply (code 2xx): %s\n", response);
    //         break;
    //     case 3:
    //         printf("Received positive intermediate reply (code 3xx): %s\n", response);
    //         break;
    //     case 4:
    //         printf("Received transient negative completion reply (code 4xx): %s\n", response);
    //         break;
    //     case 5:
    //         printf("Received permanent negative completion reply (code 5xx): %s\n", response);
    //         break;
    //     default:
    //         printf("Received unknown reply: %s\n", response);
    //         break;
    // }

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

int login(struct components c, char* response){
    if(send_socket(c.username, "USER") < 0){
        printf("Error sending USER command\n");
        return -1;
    }
    
    if(read_socket(response, MAX_LENGTH) < 0){
        printf("Error sending USER command\n");
        return -1;
    }

    if(response[0] != '3'){
        printf("Error sending username\n");
        return -1;
    }
    if(send_socket(c.password, "PASS") < 0){
        printf("Error sending PASS command\n");
        return -1;
    }

    if(read_socket(response, MAX_LENGTH) < 0){
        printf("Error sending PASS command\n");
        return -1;
    }
}