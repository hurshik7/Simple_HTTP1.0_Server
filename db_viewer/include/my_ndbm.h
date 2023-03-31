#ifndef DB_VIEWER_MY_NDBM_H
#define DB_VIEWER_MY_NDBM_H

#include <stdio.h>
#include <ndbm.h>
#include <ncurses.h>
#include <fcntl.h>
#include <string.h>
#include <time.h>
#include <ncurses.h>


#define DB_NAME "post_requests"


/**
 * Post request's body data
 */
typedef struct PostReqData {
    char *client_ip_addr;
    time_t access_time;
    char *req_data;
} post_req_data_t;


/**
 * Open post_request.db
 * @param db_path a string represents the path of the file
 * @return DBM pointer
 */
DBM *open_post_request_db(char *db_path);

/**
 * Prints all post requests in the post_request.db file
 * @param db DBM pointer
 */
void print_post_request_data(DBM *db);


#endif /* DB_VIEWER_MY_NDBM_H */

