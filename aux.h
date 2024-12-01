#ifndef AUX_H
#define AUX_H

// Include necessary headers
#include <stdio.h>
#include <stdlib.h>

// Define constants
#define MAX_LENGTH_HOSTNAME 256
#define MAX_LENGTH_PATH 1024
#define MAX_LENGTH_PORT 5
#define MAX_LENGTH_MESSAGE 200
#define MAX_LENGTH 200

struct components {
    char hostname[MAX_LENGTH_HOSTNAME];
    char username[MAX_LENGTH_HOSTNAME];
    char password[MAX_LENGTH_HOSTNAME];
    char path[MAX_LENGTH_PATH];
    char filename[MAX_LENGTH_HOSTNAME];
};

int parse_url(struct components *c, char* url);
// int compute_response(char* message, char* response, int response_size);
int send_socket(char* message, char* header);
int create_socket(const char* ip, int port);
char* get_ip(char* hostname);
int close_socket();
int read_socket(char* response, size_t response_size);
int login(struct components c, char* response);
int parse_pasv_response(const char* response, char* ip, int* port);
#endif