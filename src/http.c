#include "http.h"
#include "util.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>


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

void parse_http_req(const char* buf, int fd)
{
    http_req_t req;
    init_http_req(&req);

    uint32_t line_count = 0;
    char** req_lines = tokenize_malloc(buf, CRLF, &line_count);
    if (line_count < 1) {
        perror("wrong request (line_count)");
        // TODO implement error handling and delete perror above
        return;
    }

    char* request_line = req_lines[0];
    if (parse_req_first_line(request_line, &req) != 0) {
        perror("wrong request (first line)");
        // TODO implement error handling and delete perror above
        return;
    }

    printf("method: %d\n", req.version);
    printf("uri: %s\n", req.uri);
    printf("version: %d\n", req.version);

    if (req.version != HTTP_VERSION_10) {
        // TODO handle unsupported version
    }

    if (req.method > HTTP_METHOD_POST || req.method < 0) {
        // TODO handle unsupported method
    }

    // TODO do server


    // TODO send response

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
