#ifndef HTTP_SERVER_SERVER_H
#define HTTP_SERVER_SERVER_H


#define BACKLOG         (5)
#define SLEEP           (5)
#define ACCEPT_FAILURE  (-2)


#include "option_handler.h"
#include <ncurses.h>


extern WINDOW *output_win;


int create_socket(struct options* opts);
void reap(void);
void handle_connection(int fd, struct sockaddr_in client);
int handle_socket(int sock_fd);
_Noreturn void run_server(struct options *opts);


#endif //HTTP_SERVER_SERVER_H
