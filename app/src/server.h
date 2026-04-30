#ifndef SERVER_H
#define SERVER_H

#include <stdint.h>

#define DEFAULT_PORT     8080
#define BACKLOG          32
#define REQUEST_BUF_SIZE 2048

int server_run(uint16_t port);

#endif