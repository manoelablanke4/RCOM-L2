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

#endif // PART1_H