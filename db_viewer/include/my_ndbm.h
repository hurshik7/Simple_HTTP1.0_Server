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


typedef struct PostReqData {
    char *client_ip_addr;
    time_t access_time;
    char *req_data;
} post_req_data_t;


DBM *open_post_request_db(char *db_path);
void print_post_request_data(DBM *db);
void print_box_choices(WINDOW *box_win, int highlight);
WINDOW* create_box_window(void);
int navigate_box(WINDOW *box_win);


#endif /* DB_VIEWER_MY_NDBM_H */
