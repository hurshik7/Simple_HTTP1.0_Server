#ifndef CHAT_SERVER_CONVERSION_H
#define CHAT_SERVER_CONVERSION_H


#include <netinet/in.h>


/**
 * Parse string to in_port_t.
 * @param buff a string represents port number in string
 * @param radix an integer represents base
 * @return port number in in_port_t type
 */
in_port_t parse_port(const char *buff, int radix);


#endif /* CHAT_SERVER_CONVERSION_H */

