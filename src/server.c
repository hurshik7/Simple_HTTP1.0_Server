#include "http.h"
#include "server.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


extern bool server_running;


int create_socket(struct options* opts)
{
    int sock;
    struct sockaddr_in server;
    int result;

    memset(&server, 0, sizeof(server));

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("opening socket");
        return -1;
    }

    result = setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int));
    if (result < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        return -1;
    }
    result = setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, &(int){1}, sizeof(int));
    if (result < 0) {
        perror("setsockopt(SO_REUSEPORT) failed");
        return -1;
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(opts->port_in);
    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) != 0) {
        perror("binding socket");
        return -1;
    }

//    printf("Listening on port %d\n", ntohs(server.sin_port));
    printw("Listening on port %d\n", ntohs(server.sin_port));
    refresh();

    if (listen(sock, BACKLOG) < 0) {
        perror("listening");
        return -1;
    }

    return sock;
}

void handle_connection(int fd, struct sockaddr_in client)
{
    const char *client_ip_addr;
    ssize_t rval;
    char claddr[INET_ADDRSTRLEN];

    if ((client_ip_addr = inet_ntop(AF_INET, &client, claddr, sizeof(claddr))) == NULL) {
        perror("inet_ntop");
        client_ip_addr = "unknown";
    } else {
//        printf("Client connection from %s\n", client_ip_addr);
        printw("Client connection from %s\n", client_ip_addr);
        refresh();
    }

    do {
        char buf[BUFSIZ];
        memset(buf, 0, sizeof(buf));

        // read request from a client (HTTP 1.0 request)
        rval = read(fd, buf, BUFSIZ);
        if ((rval < 0)) {
            perror("reading stream message");
        } else if (rval == 0) {
//            printf("Ending connection from %s.\n", client_ip_addr);
            printw("Ending connection from %s.\n", client_ip_addr);
            refresh();
        } else {
//            printf("Client (%s) sent: %s\n", client_ip_addr, buf);
            printw("Client (%s) sent: %s\n", client_ip_addr, buf);
            refresh();
            parse_http_req(buf, fd);
        }
    } while (rval != 0);

    // close the fd
    close(fd);
}

int handle_socket(int sock_fd)
{
    int fd;
    pid_t pid;
    struct sockaddr_in client;
    socklen_t length;

    memset(&client, 0, sizeof(client));

    length = sizeof(client);
    fd = accept(sock_fd, (struct sockaddr *)&client, &length);
    if (fd < 0) {
        perror("accept");
        return ACCEPT_FAILURE;
    }

    int pipefd[2];
    if(pipe(pipefd)) {
        perror("pipe()");
        return -1;
    }
    // Set the read end of the pipe to non-blocking mode.
    fcntl(pipefd[0], F_SETFL, O_NONBLOCK);

    pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    } else if (pid == 0) {
        // child process
        close(pipefd[1] + 1); // close read
        // Redirect stdout to the write end of the pipe.
        dup2(pipefd[1], STDOUT_FILENO);
        handle_connection(fd, client);
        close(pipefd[1]);
        exit(EXIT_SUCCESS);
    } else {
        // parent process
        close(pipefd[1]); // close write
        while (wait(NULL) > 0);
        char buffer_from_child[BUFSIZ];
        ssize_t bytesRead;
        while ((bytesRead = read(pipefd[0], buffer_from_child, BUFSIZ)) > 0) {
            write(STDOUT_FILENO, buffer_from_child, bytesRead);
        }
    }
    return 0;
}

void reap(void)
{
    wait(NULL);
}

void* run_server_thread(void* arg)
{
    // Clear the screen
//    pthread_mutex_lock(&screen_lock);
    clear();
    refresh();

    // Display server output
    printw("Server output:\n");
    // Print your server stdout/stderr messages here
    printw("Server running...\n");

    // Refresh the screen to show the server output
    refresh();

//    while (server_running) {
//        // Server logic goes here
//    }
    struct options* opts = (struct options*) arg;
    opts->server_sock = create_socket(opts);
    if (opts->server_sock == -1) {
        exit(EXIT_FAILURE);
    }

    while (server_running) {
        fd_set ready;
        struct timeval to;
        int result;

        FD_ZERO(&ready);
        FD_SET(opts->server_sock, &ready);
        to.tv_sec = SLEEP;
        to.tv_usec = 0;
        if (select(opts->server_sock + 1, &ready, 0, 0, &to) < 0) {
            if (errno != EINTR) {
                perror("select");
            }
            continue;
        }
        if (FD_ISSET(opts->server_sock, &ready)) {
            result = handle_socket(opts->server_sock);
            if (result == -1) {
                exit(EXIT_FAILURE);
            }
        } else {
            printw("waiting for connections...\n");
            refresh();
        }
    }

    // Clear the screen again
    clear();
    refresh();

//    pthread_mutex_unlock(&screen_lock);
    close(opts->server_sock);
    return NULL;
}


_Noreturn void run_server(struct options *opts)
{
    opts->server_sock = create_socket(opts);
    if (opts->server_sock == -1) {
        exit(EXIT_FAILURE);
    }

    for (;;) {
        fd_set ready;
        struct timeval to;
        int result;

        FD_ZERO(&ready);
        FD_SET(opts->server_sock, &ready);
        to.tv_sec = SLEEP;
        to.tv_usec = 0;
        if (select(opts->server_sock + 1, &ready, 0, 0, &to) < 0) {
            if (errno != EINTR) {
                perror("select");
            }
            continue;
        }
        if (FD_ISSET(opts->server_sock, &ready)) {
            result = handle_socket(opts->server_sock);
            if (result == -1) {
                exit(EXIT_FAILURE);
            }
        } else {
            printf("waiting for connections...\n");
        }
    }
}
