#ifndef LIBCFE_TESTS_UTILS_H
#define LIBCFE_TESTS_UTILS_H


#include <stdlib.h>


extern void hex_str_to_bytes(const char *in, unsigned char *out);
extern void bytes_to_hex_str(const unsigned char *in, size_t size, char *out);


#endif
