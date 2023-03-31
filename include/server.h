#ifndef HTTP_SERVER_SERVER_H
#define HTTP_SERVER_SERVER_H


#define BACKLOG         (5)
#define SLEEP           (5)
#define ACCEPT_FAILURE  (-2)
#define MAX_READ_SIZE   (65535)


#include "option_handler.h"


/**
 * Create a socket.
 * @param opts an options struct
 * @return an integer represents a file descriptor (socket)
 */
int create_socket(struct options* opts);

/**
 * wait for child process.
 */
void reap(void);

/**
 * Handle a client's connection.
 * @param fd an integer (file descriptor)
 * @param client a sockaddr_in struct
 */
void handle_connection(int fd, struct sockaddr_in client);

/**
 * Handle a socket.
 * @param sock_fd an integer represents a file descriptor (socket)
 * @return an integer represents exit code
 */
int handle_socket(int sock_fd);

/**
 * Run the HTTP 1.0 server
 * @param opts a options struct
 */
_Noreturn void run_server(struct options *opts);


#endif //HTTP_SERVER_SERVER_H

