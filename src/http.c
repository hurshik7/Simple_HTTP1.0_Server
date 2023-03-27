#include "http.h"
#include "util.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>


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
            handle_head_request(fd, &req);
            break;
        case HTTP_METHOD_POST:

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
//    printf("%s\n", root);

    int file_fd = open(path, O_RDONLY);
    if (file_fd != -1) {
        // FILE FOUND
        // send headers
        send(fd, "HTTP/1.0 200 OK\r\n", 17, 0);
        char* date_header = get_date_header_str_malloc();
        const char* content_type_header = get_content_type_header(req->uri);
        char* content_size_header = get_content_size_header_malloc_or_null(path);
        char* last_modified_header = get_last_modified_header_malloc_or_null(path);
        send(fd, date_header, strlen(date_header), 0);
        send(fd, SERVER_HEADER, strlen(SERVER_HEADER), 0);
        send(fd, content_type_header, strlen(content_type_header), 0);
        if (content_size_header != NULL) {
            send(fd, content_size_header, strlen(content_size_header), 0);
        }
        if (last_modified_header != NULL) {
            send(fd, last_modified_header, strlen(last_modified_header), 0);
        }
        send(fd, CONNECTION_HEADER, strlen(CONNECTION_HEADER), 0);
        send(fd, "\r\n", strlen("\r\n"), 0);

        // send body
        ssize_t bytes_read = read(file_fd, data_to_send, BUFSIZ);
        while (bytes_read > 0) {
            write(fd, data_to_send, bytes_read);
            bytes_read = read(file_fd, data_to_send, BUFSIZ);
        }

        free(date_header);
        free(content_size_header);
        free(last_modified_header);
    } else {
        write(fd, "HTTP/1.0 404 Not Found\r\n", 23);
    }

    shutdown(fd, SHUT_RDWR);
    close(fd);
}

char* get_date_header_str_malloc(void)
{
    char date_time[50] = {'\0' };
    time_t now = time(NULL);
    struct tm* tm_now = gmtime(&now);
    strftime(date_time, sizeof(date_time), "Date: %a, %d %b %Y %H:%M:%S GMT\r\n", tm_now);
    char* ret_date_time_malloc = (char*) malloc(strlen(date_time) + 1);
    if (ret_date_time_malloc != NULL) {
        strncpy(ret_date_time_malloc, date_time, strlen(date_time) + 1);
        ret_date_time_malloc[strlen(date_time)] = '\0';
    }
    return ret_date_time_malloc;
}

const char* get_content_type_header(const char* file)
{
    const char* file_ext = get_file_extension(file);
    if (strcmp(file_ext, "html") == 0 || strcmp(file_ext, "htm") == 0) {
        return "Content-Type: text/html\r\n";
    } else if (strcmp(file_ext, "css") == 0) {
        return "Content-Type: text/css\r\n";
    } else if (strcmp(file_ext, "js") == 0) {
        return "Content-Type: application/javascript\r\n";
    }
    return "Content-Type: application/octet-stream\r\n";
}

char* get_last_modified_header_malloc_or_null(const char* file_path)
{
    struct stat file_info;
    if (stat(file_path, &file_info) == -1) {
        return NULL;
    }
    char last_modified_header[BUFSIZ] = { '\0', };
    struct tm *tm_modified = gmtime(&(file_info.st_mtime));
    strftime(last_modified_header, sizeof(last_modified_header), "Last-Modified: %a, %d %b %Y %H:%M:%S GMT\r\n", tm_modified);

    size_t len = strlen(last_modified_header);
    char* ret_str = (char*) malloc(len + 1);
    strncpy(ret_str, last_modified_header, len + 1);
    ret_str[len] = '\0';
    return ret_str;
}

char* get_content_size_header_malloc_or_null(const char* file_path)
{
    long file_size = get_file_size(file_path);
    char buffer[BUFSIZ] = { '\0', };
    if (file_size >= 0) {
        sprintf(buffer, "Content-Length: %ld\r\n", file_size);
    } else {
        return NULL;
    }
    size_t len = strlen(buffer);
    char* ret_str = (char*) malloc (len + 1);
    strncpy(ret_str, buffer, len + 1);
    ret_str[len] = '\0';
    return ret_str;
}

void handle_head_request(int fd, const http_req_t* req)
{
    char path[PATH_MAX] = { '\0', };
    char* root = getenv("PWD");
    char data_to_send[BUFSIZ];

    strcpy(path, root);
    strcat(path, req->uri);
    path[PATH_MAX - 1] = '\0';
    printf("%s\n", path);

    int file_fd = open(path, O_RDONLY);
    if (file_fd != -1) {
        // FILE FOUND
        // send headers
        send(fd, "HTTP/1.0 200 OK\r\n", 17, 0);
        char* date_header = get_date_header_str_malloc();
        const char* content_type_header = get_content_type_header(req->uri);
        char* content_size_header = get_content_size_header_malloc_or_null(path);
        char* last_modified_header = get_last_modified_header_malloc_or_null(path);

        send(fd, date_header, strlen(date_header), 0);
        send(fd, SERVER_HEADER, strlen(SERVER_HEADER), 0);
        send(fd, content_type_header, strlen(content_type_header), 0);
        if (content_size_header != NULL) {
            send(fd, content_size_header, strlen(content_size_header), 0);
        }
        if (last_modified_header != NULL) {
            send(fd, last_modified_header, strlen(last_modified_header), 0);
        }
        send(fd, CONNECTION_HEADER, strlen(CONNECTION_HEADER), 0);
        send(fd, "\r\n", strlen("\r\n"), 0);

        free(date_header);
        free(content_size_header);
    } else {
        write(fd, "HTTP/1.0 404 Not Found\r\n", 23);
    }

    shutdown(fd, SHUT_RDWR);
    close(fd);
}
