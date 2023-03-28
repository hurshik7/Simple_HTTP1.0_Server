#include "option_handler.h"
#include "server.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>


int main(int argc, char* argv[])
{
    // initiate options, and parse command line argument
    struct options opts;
    options_init(&opts);
    if (parse_arguments(argc, argv, &opts) == EXIT_FAILURE) {
        exit(EXIT_FAILURE);                                              // NOLINT(concurrency-mt-unsafe)
    }

    if (signal(SIGCHLD, (void (*)(int)) reap) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);                                             // NOLINT(concurrency-mt-unsafe)
    }

    run_server(&opts);
}

