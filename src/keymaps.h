#ifndef KEYMAPS_H
#define KEYMAPS_H

#include "keyboard.h"

enum keymap : uint8_t {
    KEYMAP_US,
    KEYMAP_FI,
};

WCHAR keycode_to_char(enum keymap keys, uint32_t key, enum keyboard_modifiers modifiers);
const uint32_t* ps2_scancodes_get(enum keymap keys);

#endif
