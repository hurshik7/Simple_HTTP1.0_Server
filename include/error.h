#ifndef CHAT_SERVER_ERROR_H
#define CHAT_SERVER_ERROR_H


#include <stddef.h>


/**
 * Print error message with an error code and exit the program.
 * @param file a string representing the name of the file in error
 * @param func a string representing the name of the function in error
 * @param line a positive integer represents the line number in error
 * @param err_code an integer of error code
 * @param exit_code an integer of exit code
 */
_Noreturn void fatal_errno(const char *file, const char *func, size_t line, int err_code, int exit_code);

/**
 * Print error message and exit the program.
 * @param file a string representing the name of the file in error
 * @param func a string representing the name of the function in error
 * @param line a positive integer represents the line number in error
 * @param msg a string represents a message about the error
 * @param exit_code an integer of exit code
 */
_Noreturn void fatal_message(const char *file, const char *func, size_t line, const char *msg, int exit_code);


#endif /* CHAT_SERVER_ERROR_H */

