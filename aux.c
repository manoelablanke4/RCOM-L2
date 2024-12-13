#include "aux.h"
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
int create_socket(const char* ip, int port){
    struct sockaddr_in server_addr;
    int sockfd;

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

int send_socket(int sockfd, char* message, char* header){
    char buffer[1024]; // Buffer para armazenar a mensagem completa
    int total_bytes;

    // Monta a mensagem no buffer
    snprintf(buffer, sizeof(buffer), "%s %s\r\n", header, message);

    // Envia a mensagem completa em uma única chamada
    total_bytes = write(sockfd, buffer, strlen(buffer));
    if (total_bytes != strlen(buffer)) {
        perror("write()");
        exit(-1);
    } 
    printf("%s\n", buffer);

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
        printf("Invalid URL: missing '/' for path or file\n");
        return -1;
    }
    strncpy(c->hostname, check, slash - check);
    c->hostname[slash - check] = '\0';

    check = slash + 1; // Avança para o caminho ou arquivo

    // Verifica se há um arquivo no caminho
    char* file_name = strrchr(check, '/');
    if (file_name == NULL) {
        // Não há subdiretórios; o que resta é o arquivo
        strcpy(c->path, ""); // Caminho vazio (diretório raiz)
        strcpy(c->filename, check);
    } else {
        // Há subdiretórios; separa caminho e arquivo
        strncpy(c->path, check, file_name - check);
        c->path[file_name - check] = '\0';
        strcpy(c->filename, file_name + 1);
    }

    return 0;
}
int read_socket(int sockfd, char* response, size_t response_size){

    char bytes;
    FILE* fp = fdopen(sockfd, "r");
    if(fp == NULL){
        perror("fdopen()");
        return -1;
    }
    while (1) {
        memset(response, 0, response_size); // Limpa o buffer
        if (fgets(response, response_size, fp) == NULL) {
            break;
        }
        if(response[3] == ' '){
            printf("%s", response);
            break;
        }
        printf("%s", response);
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
    sprintf(ip, "%d.%d.%d.%d", x1, x2, x3, x4);
    *port = p1 * 256 + p2;

    return 0;
}

int login(int sockfd, struct components c, char* response){

    if(send_socket(sockfd,c.username, "USER") < 0){
        printf("Error sending USER command\n");
        return -1;
    }
    if(read_socket(sockfd,response, MAX_LENGTH) < 0){
        printf("Error sending USER command\n");
        return -1;
    }
    if(response[0] != '3'){
        printf("Error sending username\n");
        return -1;
    }
    if(send_socket(sockfd,c.password, "PASS") < 0){
        printf("Error sending PASS command\n");
        return -1;
    }
    if(read_socket(sockfd,response, MAX_LENGTH) < 0){
        printf("Error sending PASS command\n");
        return -1;
    }
    return 0;
}

int send_retr_command(int sockfd, const char* filename, char* response, size_t response_size) {
    printf("Sending RETR command\n");
    if (send_socket(sockfd, filename, "RETR") < 0) {
        printf("Error sending RETR command\n");
        return -1;
    }

    // if (read_socket(sockfd, response, MAX_LENGTH) < 0) {
    //     printf("Error reading RETR response\n");
    //     return -1;
    // }
    if (response[0] == '1' || response[0] == '2') {
        return 0;
    } else {
        printf("Error retrieving file: %s\n", response);
        return -1;
    }

}

int download_file(int sockfd, const char* local_filename) {
    FILE* file = fopen(local_filename, "wb"); 
    if (file == NULL) {
        perror("Error opening local file");
        return -1;
    }

    char buffer[1024]; 
    ssize_t bytes_read;
    while ((bytes_read = read(sockfd, buffer, sizeof(buffer))) > 0) {
        printf("Downloading file %s\n", local_filename);
        if (bytes_read <= 0) {
        perror("Error reading from data socket");
        fclose(file);
        return -1;
    }
        if(fwrite(buffer, bytes_read,1, file) < 0){
            perror("Error writing to file");
            fclose(file);
            return -1;
        }
    }
    printf("Downloading file %s\n", local_filename);   
   

    fclose(file);
    printf("File downloaded successfully as %s\n", local_filename);

    return 0;
}
int close_socket(int sockfd) {
    if (sockfd != -1) {  // Check if socket is still open
        if (close(sockfd) < 0) {
            perror("Error closing socket");
            return -1;
        }
        sockfd = -1;  // Mark the socket as closed
    }
    return 0;
}
