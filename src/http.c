#include "http.h"
#include "util.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <unistd.h>
#include <time.h>


int init_http_req(http_req_t* req)
{
    req->method = -1;
    req->version = -1;
    req->if_modified_since_date = -1;
    memset(req->uri, '\0', sizeof(req->uri));
    req->content_length = 0;
    memset(req->content_type, '\0', sizeof(req->content_type));
    return 0;
}

int init_http_res(http_res_t* res, int res_code)
{
    res->code = res_code;
    res->content_length = 0;
    memset(res->content_type, '\0', sizeof(res->content_type));
    res->last_modified = -1;
    return 0;
}

bool is_valid_method(const char* method)
{
    const char* methods[] = {"GET", "POST", "HEAD", NULL};
    int i = 0;
    while (methods[i] != NULL) {
        if (strcmp(method, methods[i]) == 0) {
            return true;
        }
        i++;
    }
    return false;
}

void httpd(const char* buf, int fd)
{
    http_req_t req;
    init_http_req(&req);

    uint32_t line_count = 0;
    char** req_lines = tokenize_malloc(buf, CRLF, &line_count);
    if (line_count < 1) {
        perror("wrong request (line_count)");
        write(fd, "HTTP/1.0 400 Bad Request\n", BAD_REQUEST_RES_LEN);
        return;
    }

    char* request_line = req_lines[0];
    if (parse_req_first_line(request_line, &req) != 0) {
        perror("wrong request (first line)");
        write(fd, "HTTP/1.0 400 Bad Request\r\n", BAD_REQUEST_RES_LEN);
        close(fd);
        goto free_and_exit;
    } else {
        parse_request_headers_and_content(req_lines, line_count, &req);
    }

    printf("method: %d\n", req.version);
    printf("uri: %s\n", req.uri);
    printf("version: %d\n", req.version);

    if (req.version != HTTP_VERSION_10) {
        // TODO handle unsupported version
    }

    // TODO do server, send response
    switch(req.method) {
        case HTTP_METHOD_GET:
            handle_get_request(fd, &req);
            break;
        case HTTP_METHOD_HEAD:
            // ...
            break;
        case HTTP_METHOD_POST:
            handle_post_request(fd, &req);
            break;
        default:
            write(fd, "HTTP/1.0 405 Method Not Allowed\r\n", strlen("HTTP/1.0 405 Method Not Allowed\r\n"));
            close(fd);
            break;
    }

    free_and_exit:
    free_string_arr(req_lines, line_count);
}


int parse_req_first_line(const char* req_line, http_req_t* req_out)
{
    char* tok;
    char dup_req_line[BUFSIZ] = { '\0', };
    strncpy(dup_req_line, req_line, strlen(req_line));
    tok = strtok(dup_req_line, " ");
    if (tok == NULL) {
        return EXIT_WRONG_REQ;
    }
    // method
    if (strcmp(tok, "GET") == 0) {
        req_out->method = HTTP_METHOD_GET;
    } else if (strcmp(tok, "HEAD") == 0) {
        req_out->method = HTTP_METHOD_HEAD;
    } else if (strcmp(tok, "POST") == 0) {
        req_out->method = HTTP_METHOD_POST;
    } else {
        req_out->method = HTTP_METHOD_NOT_SUPPORTED;
    }

    tok = strtok(NULL, " ");
    // uri
    strncpy(req_out->uri, tok, strlen(tok));
    if (strcmp(tok, "/") == 0) {
        strcpy(req_out->uri, "/index.html");
    }
    if (tok == NULL) {
        return EXIT_WRONG_REQ;
    }

    // version
    tok = strtok(NULL, " ");
    if (tok == NULL) {
        return EXIT_WRONG_REQ;
    }
    char* version_tok = strtok(tok, "/");
    if (strcmp(version_tok, "HTTP") != 0) {
        return EXIT_WRONG_REQ;
    }
    version_tok = strtok(NULL, "/");
    if (strcmp(version_tok, "0.9") == 0) {
        req_out->version = HTTP_VERSION_09;
    } else if (strcmp(version_tok, "1.0") == 0) {
        req_out->version = HTTP_VERSION_10;
    } else if (strcmp(version_tok, "1.1") == 0) {
        req_out->version = HTTP_VERSION_11;
    } else {
        req_out->version = HTTP_WRONG_VERSION;
    }

    tok = strtok(NULL, " ");
    assert(tok == NULL);
    return 0;
}

void handle_get_request(int fd, const http_req_t* req)
{
    char path[PATH_MAX] = { '\0', };
    char* root = getenv("PWD");
    char data_to_send[BUFSIZ];

    strcpy(path, root);
    strcat(path, req->uri);
    path[PATH_MAX - 1] = '\0';
    printf("%s\n", root);

    int file_fd = open(path, O_RDONLY);
    if (file_fd != -1) {
        // FILE FOUND
        send(fd, "HTTP/1.0 200 OK\n\n", 17, 0);
        ssize_t bytes_read = read(file_fd, data_to_send, BUFSIZ);
        while (bytes_read > 0) {
            write(fd, data_to_send, bytes_read);
            bytes_read = read(file_fd, data_to_send, BUFSIZ);
        }
    } else {
        write(fd, "HTTP/1.0 404 Not Found\n", 23);
    }

    shutdown(fd, SHUT_RDWR);
    close(fd);
}

void handle_post_request(int fd, const http_req_t* req)
{
    char post_data[BUFSIZ];
    int res = parse_post_data(req, post_data, BUFSIZ);

    if (res == 0) {
        // successfully parsed POST req data

        // create the file path
        char path[PATH_MAX] = { '\0', };
        char* root = getenv("PWD");
        strcpy(path, root);
        strcat(path, req->uri);
        path[PATH_MAX - 1] = '\0';

        // save the POST req data to the HTML file
        int save_result = save_post_data_to_html(path, post_data);
        if (save_result == 0) {
            // send a response indicating success
            write(fd, "HTTP/1.0 200 OK\r\n", strlen("HTTP/1.0 200 OK\r\n"));
        } else {
            // send a response indicating an error occurred
            write(fd, "HTTP/1.0 500 Internal Server Error\r\n", strlen("HTTP/1.0 500 Internal Server Error\r\n"));
        }
    } else {
        // fail to parse POST data
        write(fd, "HTTP/1.0 400 Bad Request\r\n", strlen("HTTP/1.0 400 Bad Request\r\n"));
    }

    shutdown(fd, SHUT_RDWR);
    close(fd);
}

