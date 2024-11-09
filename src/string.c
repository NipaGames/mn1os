#include "string.h"
#include "terminal.h"

size_t strlen(const char* s) {
    size_t len = 0;
    while (s[len])
        len++;
    return len;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 != '\0' && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*) s1 - *(const unsigned char*) s2;
}

char to_lower(char c) {
    if (c >= 0x41 && c <= 0x5a)
        return c + 0x20;
    return c;
}

char to_upper(char c) {
    if (c >= 0x61 && c <= 0x7a)
        return c - 0x20;
    return c;
}

int isspace(char c) {
    return
        c == ' '    ||
        c == '\t'   ||
        c == '\n'   ||
        c == '\v'   ||
        c == '\f'   ||
        c == '\r';
}

int str_to_int(const char* s, int* err, int base) {
    if (base <= 0 || base > 16) {
        T_WRITE_DIAGNOSTIC_STUB();
        t_write("invalid base! (1-16 allowed, ");
        t_write_dec(base);
        t_write(" given)\n");
        if (err) *err = 1;
        return 0;
    }
    size_t len = strlen(s);
    if (len == 0) {
        if (err) *err = 1;
        return 0;
    }
    size_t i = 0;
    int sign = 1;
    if (s[i] == '-') {
        sign = -1;
        i++;
        if (i == len) {
            if (err) *err = 1;
            return 0;
        }
    }
    unsigned int n = 0;
    unsigned int cutoff = (sign == -1) ? -(unsigned int) INT_MIN : INT_MAX;
    int cutlim = cutoff % (unsigned int) base;
    cutoff /= (unsigned int) base;
    while (i < len) {
        char c = to_lower(s[i]);
        if (i == 0 && i != len - 1 && c == '-') {
            n *= -1;
            i++;
            continue;
        }
        int digit = -1;
        if (c >= '0' && c <= '9')
            digit = c - '0';
        else if (c >= 'a' && c <= 'f')
            digit = 0xa + (c - 'a');
        if (digit == -1 || digit >= base || n > cutoff || (n == cutoff && digit > cutlim)) {
            if (err) *err = 1;
            return 0;
        }
        n *= base;
        n += digit;
        i++;
    }
    n *= sign;
    if (err) *err = 0;
    return (int) n;
}

