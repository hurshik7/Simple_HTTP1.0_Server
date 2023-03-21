#ifndef HTTP_SERVER_UTIL_H
#define HTTP_SERVER_UTIL_H


#include <stdint.h>


#define DEFAULT_ARGS (10)


char** tokenize_malloc(const char* str, const char* delim, uint32_t* out_count);
void free_string_arr(char** str_arr, uint32_t count);


#endif //HTTP_SERVER_UTIL_H