#ifndef HTTP_SERVER_HTTP_H
#define HTTP_SERVER_HTTP_H


#include <time.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>


#define CONTENT_TYPE_LEN (64)
#define CRLF "\r\n"

#define HTTP_VERSION_10 (10)
#define HTTP_VERSION_09 (9)
#define HTTP_VERSION_11 (11)
#define HTTP_WRONG_VERSION (-1)

#define HTTP_METHOD_GET (0)
#define HTTP_METHOD_HEAD (1)
#define HTTP_METHOD_POST (2)
#define HTTP_METHOD_NOT_SUPPORTED (99)

#define RES_TYPE_HTML "text/html"
#define RES_TYPE_CSS "text/css"
#define RES_TYPE_JS "text/js"
#define RES_TYPE_PNG "image/png"

#define EXIT_WRONG_REQ (-5)
#define BAD_REQUEST_RES_LEN (25)


/**
 * http_req struct contains the information of a header from a http client.
 */
typedef struct http_req {
    int method;
    int version;
    char uri[PATH_MAX + 1];
    time_t if_modified_since_date;
    char content[1024];
    uint32_t content_length;
    char content_type[CONTENT_TYPE_LEN];
} http_req_t;

/**
 * http_res struct contains the information of a response.
 */
typedef struct http_res {
    int code;
    time_t last_modified;
    char content_type[CONTENT_TYPE_LEN];
    uint32_t content_length;
} http_res_t;


static const char* SERVER_HEADER = "Server: COMP4981_HTTP_SERVER/1.0\n";
static const char* CONNECTION_HEADER = "Connection: close\n";


int init_http_req(http_req_t* req);
int init_http_res(http_res_t* res, int res_code);
void httpd(const char* buf, int fd, const char* client_ip_addr);
int parse_req_first_line(const char* req_line, http_req_t* req_out);
void handle_get_request(int fd, const http_req_t* req);
void handle_post_request(int fd, const http_req_t* req, const char* client_ip_addr);
void handle_head_request(int fd, const http_req_t* req);
int parse_request_headers_and_content(char **req_lines, uint32_t line_count, http_req_t *req);
int parse_post_data(const http_req_t *req, char *post_data, size_t post_data_len);
int save_post_data_to_html(const char *html_file_path, const char *post_data);
char* get_date_header_str_malloc(void);
const char* get_content_type_header(const char* file);
char* get_last_modified_header_malloc_or_null(const char* file_path);
char* get_content_size_header_malloc_or_null(const char* file_path);

#endif //HTTP_SERVER_HTTP_H

