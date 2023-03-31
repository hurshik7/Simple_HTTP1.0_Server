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


/**
 * Init http_req_t struct.
 * @param req a pointer to http_req_t struct.
 * @return an integer represents a status code
 */
int init_http_req(http_req_t* req);

/**
 * Init http_res_t struct
 * @param res a pointer to http_res_t struct
 * @param res_code an integer represents HTTP 1.0 response code
 * @return an integer
 */
int init_http_res(http_res_t* res, int res_code);

/**
 * Process a http 1.0 request.
 * @param buf a string contains the request
 * @param fd a integer represents a socket
 * @param client_ip_addr a string represents a client's ip address
 */
void httpd(const char* buf, int fd, const char* client_ip_addr);

/**
 * Parse the first line of a HTTP 1.0 request.
 * @param req_line a string contains the first line
 * @param req_out a pointer to http_req_t struct
 * @return an integer represents exit code
 */
int parse_req_first_line(const char* req_line, http_req_t* req_out);

/**
 * Handle a GET request.
 * @param fd an integer represents a socket
 * @param req a pointer to a http_req_t struct
 */
void handle_get_request(int fd, const http_req_t* req);

/**
 * Handle POST request.
 * @param fd an integer represents a socket
 * @param req a pointer to a http_req_t struct
 * @param client_ip_addr a string represents an ip address of a client
 */
void handle_post_request(int fd, const http_req_t* req, const char* client_ip_addr);

/**
 * Handle HEAD request.
 * @param fd an integer represents a socket
 * @param req a pointer to a http_req_t struct
 */
void handle_head_request(int fd, const http_req_t* req);

/**
 * Parse request headers and content.
 * @param req_lines a string array contains a http request
 * @param line_count a 32 bits integer represents the number of lines
 * @param req a pointer to a http_req_t struct
 * @return an integer
 */
int parse_request_headers_and_content(char **req_lines, uint32_t line_count, http_req_t *req);

/**
 * Parse a POST request's body data
 * @param req a pointer to http_req_t struct
 * @param post_data a string contains the data of the POST request
 * @param post_data_len an unsigned integer represents the length of the data (content-length)
 * @return an integer
 */
int parse_post_data(const http_req_t *req, char *post_data, size_t post_data_len);

/**
 * Save a data of a POST request into a html file.
 * @param html_file_path a string represents the path of a html file
 * @param post_data a string contains the data of a POST request
 * @return an integer
 */
int save_post_data_to_html(const char *html_file_path, const char *post_data);

/**
 * Get data header string for a response.
 * @return a pointer to char (dynamically allocated, so it has to be freed after use)
 */
char* get_date_header_str_malloc(void);

/**
 * Get content type in a header for a response.
 * @param file a string
 * @return a string
 */
const char* get_content_type_header(const char* file);

/**
 * Get last modified header for a response
 * @param file_path a string contains the path of a file
 * @return a string
 */
char* get_last_modified_header_malloc_or_null(const char* file_path);

/**
 * Get a content-size header for a response
 * @param file_path a string contains the path of a file
 * @return a string
 */
char* get_content_size_header_malloc_or_null(const char* file_path);

#endif //HTTP_SERVER_HTTP_H

