#ifndef CHAT_SERVER_OPTION_HANDLER_H
#define CHAT_SERVER_OPTION_HANDLER_H


#include <arpa/inet.h>
#include <limits.h>


#define DEFAULT_PORT (80)
#define MAX_ARG_COUNT (3)


/**
 * Options struct contains port number, a file descriptor for socket.
 */
struct options
{
    in_port_t port_in;
    int server_sock;
};

/**
 * Initiate struct options.
 * @param opts a struct represents options
 */
void options_init(struct options *opts);

/**
 * Parse command line arguments.
 * @param argc an integer represents the number of command line arguments
 * @param argv an array of char* contains the arguments as strings
 * @param opts a struct represents options
 */
int parse_arguments(int argc, char *argv[], struct options *opts);


#endif /* CHAT_SERVER_OPTION_HANDLER_H */

