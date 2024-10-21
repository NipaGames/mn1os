#ifndef UTF8_H
#define UTF8_H

#include "types.h"

#define WCHAR uint16_t

int     is_utf8(WCHAR c);
uint8_t utf8_to_cp437(WCHAR c);

#endif
