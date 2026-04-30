#include <stdio.h>
#include <stdlib.h>
#include "server.h"

int main(int argc, char *argv[])
{
    uint16_t port = DEFAULT_PORT;

    if (argc == 2) {
        int p = atoi(argv[1]);
        if (p <= 0 || p > 65535) {
            fprintf(stderr, "invalid port: %s\n", argv[1]);
            return EXIT_FAILURE;
        }
        port = (uint16_t)p;
    }

    return server_run(port) == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}