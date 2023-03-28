#include "my_ndbm.h"
#include <netinet/in.h> // for INET6_ADDRSTRLEN


// replace print(), perror() with printw() + refresh()
#include "ncurses_ui.h"
#define printf(...) do { \
    printw(__VA_ARGS__); \
    refresh(); \
} while (0)
#define perror(...) do { \
    printw(__VA_ARGS__); \
    refresh(); \
} while (0)


DBM *open_post_request_db(char *db_path)
{
    DBM *db = dbm_open(db_path, O_RDONLY, 0);
    if (db == NULL) {
        perror("Error opening NDBM database");
    }
    return db;
}

void print_post_request_data(DBM *db)
{
    if (db == NULL) {
        perror("Error: NULL DBM pointer");
        return;
    }

    printf("Client IP Addr | Access Time          | POST Data\n");
    printf("--------------------------------------------------\n");

    datum key, value;
    for (key = dbm_firstkey(db); key.dptr != NULL; key = dbm_nextkey(db)) {
        value = dbm_fetch(db, key);

        if (value.dptr != NULL) {
            post_req_data_t post_req_data;
            char *data_ptr = value.dptr;

            // Deserialize the access_time
            sscanf(data_ptr, "%lld", (long long *)&post_req_data.access_time);
            data_ptr = strchr(data_ptr, '\n') + 1; // Move the pointer to the beginning of the post_data

            // Deserialize the post_data
            post_req_data.req_data = data_ptr;

            // Get the client IP address from the key
            char client_ip[INET6_ADDRSTRLEN] = {0};
            strncpy(client_ip, (char *)key.dptr, strchr((char *)key.dptr, '_') - (char *)key.dptr);
            post_req_data.client_ip_addr = client_ip;

            char time_str[32];
            strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&post_req_data.access_time));
            printf("%-15s | %-20s | %s\n", post_req_data.client_ip_addr, time_str, post_req_data.req_data);
        }
    }
}
