#ifndef DB_VIEWER_MY_NDBM_H
#define DB_VIEWER_MY_NDBM_H

#include <stdio.h>
#include <ndbm.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>


#define DB_NAME "post_requests"


typedef struct PostReqData {
    char *client_ip_addr;
    time_t access_time;
    char *req_data;
} post_req_data_t;


DBM *open_post_request_db(const char *db_path);
void print_post_request_data(DBM *db);


#endif /* DB_VIEWER_MY_NDBM_H */
