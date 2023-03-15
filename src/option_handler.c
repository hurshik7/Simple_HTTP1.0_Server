#include "conversion.h"
#include "option_handler.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void options_init(struct options *opts)
{
    memset(opts, 0, sizeof(struct options));
    opts->port_in       = DEFAULT_PORT;
    opts->server_sock   = STDIN_FILENO;
}

int parse_arguments(int argc, char *argv[], struct options *opts)    // NOLINT(readability-function-cognitive-complexity)
{
    if (argc > MAX_ARG_COUNT || argc % 2 == 0)
    {
        fprintf(stderr, "Usage: %s [-p port_number]\n", argv[0]);
        return EXIT_FAILURE;
    }

    int c;

    while((c = getopt(argc, argv, "p:")) != -1)   // NOLINT(concurrency-mt-unsafe)
    {
        switch(c) {
            case 'p':
            {
                opts->port_in = parse_port(optarg, 10); // NOLINT(cppcoreguidelines-avoid-magic-numbers,readability-magic-numbers)
                break;
            }
            case ':':
            {
                fprintf(stderr, "Option requires an operand\n");
                return EXIT_FAILURE;
            }
            case '?':
            {
                fprintf(stderr, "parse_arguments: Unknown\n");
                return EXIT_FAILURE;
            }
            default:
            {
                assert("should not get here");
            }
        }
    }

    if (optind < argc)
    {
        fprintf(stderr, "Usage: %s [-p port_number]\n", argv[0]);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

