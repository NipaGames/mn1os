#ifndef STRING_H
#define STRING_H

#include "types.h"

size_t strlen(const char* s);

// works only on ascii
char to_lower(char c);

int str_to_int(const char* s, int* err, int base);

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

