#ifndef PART1_H
#define PART1_H

// Include necessary headers
#include <stdio.h>
#include <stdlib.h>

// Define constants
#define MAX_LENGTH_HOSTNAME 256
#define MAX_LENGTH_PATH 1024
#define MAX_LENGTH_PORT 5

struct components {
    char hostname[MAX_LENGTH_HOSTNAME];
    char username[MAX_LENGTH_HOSTNAME];
    char password[MAX_LENGTH_HOSTNAME];
    char path[MAX_LENGTH_PATH];
    char filename[MAX_LENGTH_HOSTNAME];
};

int parse_url(struct components *c, char* url);
int compute_response(char* message, char* response, int response_size);
int send_socket(char* message);
int create_socket(const char* ip, int port);
char* get_ip(char* hostname);


#endif // PART1_H