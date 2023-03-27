#include "http.h"
#include "server.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


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

    printf("Listening on port %d\n", ntohs(server.sin_port));

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
        printf("Client connection from %s\n", client_ip_addr);
    }

    char buf[MAX_READ_SIZE];
    memset(buf, 0, MAX_READ_SIZE);

    // read request from a client (HTTP 1.0 request)
    rval = read(fd, buf, MAX_READ_SIZE);
    if ((rval < 0)) {
        perror("reading stream message");
    } else if (rval == 0) {
        printf("Ending connection from %s.\n", client_ip_addr);
    } else {
        printf("Client (%s) sent: %s\n", client_ip_addr, buf);
        httpd(buf, fd, client_ip_addr);
    }

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

    pid = fork();
    if (pid < 0) {
        perror("fork");
        return -1;
    } else if (pid == 0) {
        // child process
        handle_connection(fd, client);
        exit(EXIT_SUCCESS);
    } else {
        // parent process
        close(fd);
    }
    return 0;
}

void reap(void)
{
    wait(NULL);
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
