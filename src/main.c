#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


#define BACKLOG         (5)
#define SLEEP           (5)
#define DEFAULT_PORT    (80)


int create_socket(void);
void reap(void);
void handle_connection(int fd, struct sockaddr_in client);
void handle_socket(int s);


int main(int argc, char *argv[])
{
    int sock_fd;

    if (signal(SIGCHLD, (void (*)(int)) reap) == SIG_ERR) {
        perror("signal");
        exit(EXIT_FAILURE);
    }

    sock_fd = create_socket();

    for (;;) {
        fd_set ready;
        struct timeval to;

        FD_ZERO(&ready);
        FD_SET(sock_fd, &ready);
        to.tv_sec = SLEEP;
        to.tv_usec = 0;
        if (select(sock_fd + 1, &ready, 0, 0, &to) < 0) {
            if (errno != EINTR) {
                perror("select");
            }
            continue;
        }
        if (FD_ISSET(sock_fd, &ready)) {
            handle_socket(sock_fd);
        } else {
            printf("waiting for connections...\n");
        }
    }
}

int create_socket(void)
{
    int sock;
    struct sockaddr_in server;

    memset(&server, 0, sizeof(server));

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("opening socket");
        exit(EXIT_FAILURE);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(DEFAULT_PORT);
    if (bind(sock, (struct sockaddr *)&server, sizeof(server)) != 0) {
        perror("binding socket");
        exit(EXIT_FAILURE);
    }

    printf("Listening on port %d\n", ntohs(server.sin_port));

    if (listen(sock, BACKLOG) < 0) {
        perror("listening");
        exit(EXIT_FAILURE);
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

    do {
        char buf[BUFSIZ];
        memset(buf, 0, sizeof(buf));

        // read request from a client (HTTP 1.0 request)
        rval = read(fd, buf, BUFSIZ);
        if ((rval < 0)) {
            perror("reading stream message");
        } else if (rval == 0) {
            printf("Ending connection from %s.\n", client_ip_addr);
        } else {
            printf("Client (%s) sent: %s\n", client_ip_addr, buf);
        }

        // TODO
        // if there is a something represents end of request
        // break this look

    } while (rval != 0);

    // TODO
    // send a response to the client

    // close the fd
    close(fd);
}

void handle_socket(int s)
{
    int fd;
    pid_t pid;
    struct sockaddr_in client;
    socklen_t length;

    memset(&client, 0, sizeof(client));

    length = sizeof(client);
    fd = accept(s, (struct sockaddr *)&client, &length);
    if (fd < 0) {
        perror("accept");
        return;
    }

    pid = fork();
    if (pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        // child process
        handle_connection(fd, client);
        exit(EXIT_SUCCESS);
    } else {
        // parent process
    }
}

void reap(void)
{
    wait(NULL);
}
