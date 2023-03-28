#include "my_ndbm.h"

DBM *open_post_request_db(char* db_path) {
    DBM *db = dbm_open(db_path, O_CREAT | O_RDWR | O_SYNC | O_APPEND, 0644);
    if (db == NULL) {
        perror("Error opening NDBM database");
    }
    return db;
}

int insert_post_request_data(DBM *db, post_req_data_t *post_req_data) {
    if (db == NULL || post_req_data == NULL) {
        perror("Error: NULL DBM pointer or NULL post_req_data_t pointer");
        return -1;
    }

    // Create a unique key for the record using client_ip_addr and access_time
    char key_str[256];
    snprintf(key_str, sizeof(key_str), "%s_%lld", post_req_data->client_ip_addr, (long long)post_req_data->access_time);

    // Serialize the post_req_data_t struct
    char value_str[BUFSIZ];
    snprintf(value_str, sizeof(value_str), "%lld\n%s", (long long)post_req_data->access_time, post_req_data->req_data);

    // Save the serialized data to the NDBM database
    datum key, value;
    key.dptr = key_str;
    key.dsize = strlen(key_str) + 1;
    value.dptr = value_str;
    value.dsize = strlen(value_str) + 1;

    int result = dbm_store(db, key, value, DBM_INSERT);
    if (result != 0) {
        perror("Error inserting data into NDBM database");
        return -1;
    }

    return 0;
}
