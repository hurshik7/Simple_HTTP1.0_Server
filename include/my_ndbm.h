#ifndef HTTP_SERVER_MY_NDBM_H
#define HTTP_SERVER_MY_NDBM_H


#include <stdio.h>
#include <ndbm.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>


#define DB_NAME "post_requests"


/**
 * PostReqData struct.
 * It contains ip address of a client (ipv4) as a string, access time as time_t, and the data.
 */
typedef struct PostReqData {
    const char* client_ip_addr;
    time_t access_time;
    char* req_data;
} post_req_data_t;


/**
 * Open post_request.db database.
 * @param db_path a string represents the path of the database
 * @return DBM pointer
 */
DBM *open_post_request_db(char* db_path);

/**
 * Insert a data into the post_request.db
 * @param db DBM pointer
 * @param post_req_data a pointer to PostReqData struct
 * @return an integer represents a status code
 */
int insert_post_request_data(DBM *db, post_req_data_t *post_req_data);


#endif //HTTP_SERVER_MY_NDBM_H

