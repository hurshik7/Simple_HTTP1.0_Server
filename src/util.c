#include "util.h"
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>


char** tokenize_malloc(const char* str, const char* delim, uint32_t* out_count)
{
    char* temp_str;
    char* token;
    size_t str_length = strlen(str);
    size_t word_count = 0;
    size_t ret_size = DEFAULT_ARGS;
    size_t word_length;
    char** ret;
    void* temp = NULL;

    temp_str = malloc((str_length + 1) * sizeof(char));
    strcpy(temp_str, str);

    ret = malloc(ret_size * sizeof(char*));

    token = strtok(temp_str, delim);                            // NOLINT(concurrency-mt-unsafe)
    if (token == NULL) {
        ret[0] = NULL;
        goto free_and_exit;
    }

    while (token != NULL) {
        word_count++;
        word_length = strlen(token);

        if (word_count == ret_size - 1) {
            ret_size += ret_size;
            temp = realloc(ret, ret_size * sizeof(char*));
            if (temp != NULL) {
                ret = temp;
            }
        }
        ret[word_count - 1] = malloc((word_length + 1) * sizeof(char));
        strcpy(ret[word_count - 1], token);
        if (ret[word_count - 1][strlen(ret[word_count - 1]) - 1] == '\n') {
            ret[word_count - 1][strlen(ret[word_count - 1]) - 1] = '\0';
        }

        token = strtok(NULL, delim);                            // NOLINT(concurrency-mt-unsafe)
    }

    ret[word_count] = NULL;
    *out_count = word_count;

    free_and_exit:
    free(temp_str);
    temp_str = NULL;
    return ret;
}

void free_string_arr(char** str_arr, uint32_t count)
{
    for (uint32_t i = 0; i < count; i++) {
        free(str_arr[i]);
        str_arr[i] = NULL;
    }
    free(str_arr);
    str_arr = NULL;
}

const char* get_file_extension(const char* filename)
{
    const char *dot = strrchr(filename, '.');
    if (!dot || dot == filename) {
        return NULL; // No extension found or hidden file (e.g. ".gitignore")
    }
    return dot + 1;
}

long get_file_size(const char* filepath)
{
    struct stat file_info;

    if (stat(filepath, &file_info) != 0) {
        // file not found, permission issues, etc.
        return -1;
    }
    return (long) file_info.st_size;
}
