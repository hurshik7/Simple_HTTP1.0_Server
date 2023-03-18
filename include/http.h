#ifndef HTTP_SERVER_HTTP_H
#define HTTP_SERVER_HTTP_H


#include <time.h>
#include <limits.h>
#include <stdint.h>


#define CONTENT_TYPE_LEN (64)


/**
 * http_req struct contains the information of a header from a http client.
 */
typedef struct http_req {
    int method;
    char uri[PATH_MAX + 1];
    time_t if_modified_since_date;
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



#endif //HTTP_SERVER_HTTP_H
