#ifndef STRING_H
#define STRING_H

#include "types.h"

size_t  strlen(const char* s);
int     strcmp(const char* s1, const char* s2);

// works only on ascii
char    to_lower(char c);
char    to_upper(char c);
int     isspace(char c);

int     str_to_int(const char* s, int* err, int base);

static inline int str_to_int_dec(const char* s, int* err) {
    return str_to_int(s, err, 10);
}
static inline int str_to_int_hex(const char* s, int* err) {
    return str_to_int(s, err, 16);
}
static inline int str_to_int_bin(const char* s, int* err) {
    return str_to_int(s, err, 2);
}

#endif

