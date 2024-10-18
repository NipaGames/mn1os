#include "memory.h"

void memset(void* ptr, int val, size_t len) {
    uint8_t* p = ptr;
    while (len > 0) {
        *p++ = val;
        len--;
    }
}
