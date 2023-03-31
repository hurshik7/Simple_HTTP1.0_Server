#ifndef HTTP_SERVER_UTIL_H
#define HTTP_SERVER_UTIL_H


#include <stdint.h>


#define DEFAULT_ARGS (10)


/**
 * Return a string array which contains tokens.
 * @param str a string will be tokenized
 * @param delim a string contains delimiters
 * @param out_count 32 bits unsigned integer will point to the number of tokens
 * @return a string array
 */
char** tokenize_malloc(const char* str, const char* delim, uint32_t* out_count);

/**
 * Free dynamically allocated memory in a string array.
 * @param str_arr a string array
 * @param count the number of string in the array
 */
void free_string_arr(char** str_arr, uint32_t count);

/**
 * Get file extension.
 * @param filename a string represents a filename
 * @return a string represents the extension of the file
 */
const char* get_file_extension(const char* filename);

/**
 * Get the size of a file
 * @param filepath a string represents the path of the file
 * @return a long integer
 */
long get_file_size(const char* filepath);


#endif //HTTP_SERVER_UTIL_H

