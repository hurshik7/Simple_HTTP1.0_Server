#ifndef HTTP_SERVER_MY_NDBM_H
#define HTTP_SERVER_MY_NDBM_H


#include <stdio.h>
#include <ndbm.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>


#define DB_NAME "post_requests"


typedef struct PostReqData {
    const char* client_ip_addr;
    time_t access_time;
    char* req_data;
} post_req_data_t;


DBM *open_post_request_db(char* db_path);
int insert_post_request_data(DBM *db, post_req_data_t *post_req_data);


#endif //HTTP_SERVER_MY_NDBM_H

