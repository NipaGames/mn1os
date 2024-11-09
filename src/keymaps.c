#include "keymaps.h"
#include "string.h"
#include "utf8.h"

static const uint32_t g_ps2_scancodes_us[0xff] = {
    [0x02] = KEY_1 | KEY_PRESS,
    [0x03] = KEY_2 | KEY_PRESS,
    [0x04] = KEY_3 | KEY_PRESS,
    [0x05] = KEY_4 | KEY_PRESS,
    [0x06] = KEY_5 | KEY_PRESS,
    [0x07] = KEY_6 | KEY_PRESS,
    [0x08] = KEY_7 | KEY_PRESS,
    [0x09] = KEY_8 | KEY_PRESS,
    [0x0a] = KEY_9 | KEY_PRESS,
    [0x0b] = KEY_0 | KEY_PRESS,
    [0x0c] = KEY_DASH | KEY_PRESS,
    [0x0d] = KEY_EQUALS | KEY_PRESS,
    [0x0e] = KEY_BACKSPACE | KEY_PRESS,
    [0x0f] = KEY_TAB | KEY_PRESS,
    [0x10] = KEY_Q | KEY_PRESS,
    [0x11] = KEY_W | KEY_PRESS,
    [0x12] = KEY_E | KEY_PRESS,
    [0x13] = KEY_R | KEY_PRESS,
    [0x14] = KEY_T | KEY_PRESS,
    [0x15] = KEY_Y | KEY_PRESS,
    [0x16] = KEY_U | KEY_PRESS,
    [0x17] = KEY_I | KEY_PRESS,
    [0x18] = KEY_O | KEY_PRESS,
    [0x19] = KEY_P | KEY_PRESS,
    [0x1a] = KEY_SQUARE_BRACKET_OPEN | KEY_PRESS,
    [0x1b] = KEY_SQUARE_BRACKET_CLOSE | KEY_PRESS,
    [0x1c] = KEY_ENTER | KEY_PRESS,
    [0x1d] = KEY_CTRL_LEFT | KEY_PRESS,
    [0x1e] = KEY_A | KEY_PRESS,
    [0x1f] = KEY_S | KEY_PRESS,
    [0x20] = KEY_D | KEY_PRESS,
    [0x21] = KEY_F | KEY_PRESS,
    [0x22] = KEY_G | KEY_PRESS,
    [0x23] = KEY_H | KEY_PRESS,
    [0x24] = KEY_J | KEY_PRESS,
    [0x25] = KEY_K | KEY_PRESS,
    [0x26] = KEY_L | KEY_PRESS,
    [0x27] = KEY_SEMICOLON | KEY_PRESS,
    [0x28] = KEY_SINGLE_QUOTE | KEY_PRESS,
    [0x29] = KEY_BACKTICK | KEY_PRESS,
    [0x2a] = KEY_SHIFT_LEFT | KEY_PRESS,
    [0x2b] = KEY_BACKSLASH | KEY_PRESS,
    [0x2c] = KEY_Z | KEY_PRESS,
    [0x2d] = KEY_X | KEY_PRESS,
    [0x2e] = KEY_C | KEY_PRESS,
    [0x2f] = KEY_V | KEY_PRESS,
    [0x30] = KEY_B | KEY_PRESS,
    [0x31] = KEY_N | KEY_PRESS,
    [0x32] = KEY_M | KEY_PRESS,
    [0x33] = KEY_COMMA | KEY_PRESS,
    [0x34] = KEY_PERIOD | KEY_PRESS,
    [0x35] = KEY_SLASH | KEY_PRESS,
    [0x36] = KEY_SHIFT_RIGHT | KEY_PRESS,
    [0x37] = KEY_KEYPAD_ASTERISK | KEY_PRESS,
    [0x38] = KEY_ALT_LEFT | KEY_PRESS,
    [0x39] = KEY_SPACE | KEY_PRESS,
    [0x3a] = KEY_CAPS_LOCK | KEY_PRESS,
    [0x3b] = KEY_F1 | KEY_PRESS,
    [0x3c] = KEY_F2 | KEY_PRESS,
    [0x3d] = KEY_F3 | KEY_PRESS,
    [0x3e] = KEY_F4 | KEY_PRESS,
    [0x3f] = KEY_F5 | KEY_PRESS,
    [0x40] = KEY_F6 | KEY_PRESS,
    [0x41] = KEY_F7 | KEY_PRESS,
    [0x42] = KEY_F8 | KEY_PRESS,
    [0x43] = KEY_F9 | KEY_PRESS,
    [0x44] = KEY_F10 | KEY_PRESS,
    [0x45] = KEY_NUM_LOCK | KEY_PRESS,
    [0x46] = KEY_SCROLL_LOCK | KEY_PRESS,
    [0x47] = KEY_KEYPAD_7 | KEY_PRESS,
    [0x48] = KEY_KEYPAD_8 | KEY_PRESS,
    [0x49] = KEY_KEYPAD_9 | KEY_PRESS,
    [0x4a] = KEY_KEYPAD_DASH | KEY_PRESS,
    [0x4b] = KEY_KEYPAD_4 | KEY_PRESS,
    [0x4c] = KEY_KEYPAD_5 | KEY_PRESS,
    [0x4d] = KEY_KEYPAD_6 | KEY_PRESS,
    [0x4e] = KEY_KEYPAD_PLUS | KEY_PRESS,
    [0x4f] = KEY_KEYPAD_1 | KEY_PRESS,
    [0x50] = KEY_KEYPAD_2 | KEY_PRESS,
    [0x51] = KEY_KEYPAD_3 | KEY_PRESS,
    [0x52] = KEY_KEYPAD_0 | KEY_PRESS,
    [0x53] = KEY_KEYPAD_PERIOD | KEY_PRESS,
    [0x57] = KEY_F11 | KEY_PRESS,
    [0x58] = KEY_F12 | KEY_PRESS,
    [0x82] = KEY_1 | KEY_RELEASE,
    [0x83] = KEY_2 | KEY_RELEASE,
    [0x84] = KEY_3 | KEY_RELEASE,
    [0x85] = KEY_4 | KEY_RELEASE,
    [0x86] = KEY_5 | KEY_RELEASE,
    [0x87] = KEY_6 | KEY_RELEASE,
    [0x88] = KEY_7 | KEY_RELEASE,
    [0x89] = KEY_8 | KEY_RELEASE,
    [0x8a] = KEY_9 | KEY_RELEASE,
    [0x8b] = KEY_0 | KEY_RELEASE,
    [0x8c] = KEY_DASH | KEY_RELEASE,
    [0x8d] = KEY_EQUALS | KEY_RELEASE,
    [0x8e] = KEY_BACKSPACE | KEY_RELEASE,
    [0x8f] = KEY_TAB | KEY_RELEASE,
    [0x90] = KEY_Q | KEY_RELEASE,
    [0x91] = KEY_W | KEY_RELEASE,
    [0x92] = KEY_E | KEY_RELEASE,
    [0x93] = KEY_R | KEY_RELEASE,
    [0x94] = KEY_T | KEY_RELEASE,
    [0x95] = KEY_Y | KEY_RELEASE,
    [0x96] = KEY_U | KEY_RELEASE,
    [0x97] = KEY_I | KEY_RELEASE,
    [0x98] = KEY_O | KEY_RELEASE,
    [0x99] = KEY_P | KEY_RELEASE,
    [0x9a] = KEY_SQUARE_BRACKET_OPEN | KEY_RELEASE,
    [0x9b] = KEY_SQUARE_BRACKET_CLOSE | KEY_RELEASE,
    [0x9c] = KEY_ENTER | KEY_RELEASE,
    [0x9d] = KEY_CTRL_LEFT | KEY_RELEASE,
    [0x9e] = KEY_A | KEY_RELEASE,
    [0x9f] = KEY_S | KEY_RELEASE,
    [0xa0] = KEY_D | KEY_RELEASE,
    [0xa1] = KEY_F | KEY_RELEASE,
    [0xa2] = KEY_G | KEY_RELEASE,
    [0xa3] = KEY_H | KEY_RELEASE,
    [0xa4] = KEY_J | KEY_RELEASE,
    [0xa5] = KEY_K | KEY_RELEASE,
    [0xa6] = KEY_L | KEY_RELEASE,
    [0xa7] = KEY_SEMICOLON | KEY_RELEASE,
    [0xa8] = KEY_SINGLE_QUOTE | KEY_RELEASE,
    [0xa9] = KEY_BACKTICK | KEY_RELEASE,
    [0xaa] = KEY_SHIFT_LEFT | KEY_RELEASE,
    [0xab] = KEY_BACKSLASH | KEY_RELEASE,
    [0xac] = KEY_Z | KEY_RELEASE,
    [0xad] = KEY_X | KEY_RELEASE,
    [0xae] = KEY_C | KEY_RELEASE,
    [0xaf] = KEY_V | KEY_RELEASE,
    [0xb0] = KEY_B | KEY_RELEASE,
    [0xb1] = KEY_N | KEY_RELEASE,
    [0xb2] = KEY_M | KEY_RELEASE,
    [0xb3] = KEY_COMMA | KEY_RELEASE,
    [0xb4] = KEY_PERIOD | KEY_RELEASE,
    [0xb5] = KEY_SLASH | KEY_RELEASE,
    [0xb6] = KEY_SHIFT_RIGHT | KEY_RELEASE,
    [0xb7] = KEY_KEYPAD_ASTERISK | KEY_RELEASE,
    [0xb8] = KEY_ALT_LEFT | KEY_RELEASE,
    [0xb9] = KEY_SPACE | KEY_RELEASE,
    [0xba] = KEY_CAPS_LOCK | KEY_RELEASE,
    [0xbb] = KEY_F1 | KEY_RELEASE,
    [0xbc] = KEY_F2 | KEY_RELEASE,
    [0xbd] = KEY_F3 | KEY_RELEASE,
    [0xbe] = KEY_F4 | KEY_RELEASE,
    [0xbf] = KEY_F5 | KEY_RELEASE,
    [0xc0] = KEY_F6 | KEY_RELEASE,
    [0xc1] = KEY_F7 | KEY_RELEASE,
    [0xc2] = KEY_F8 | KEY_RELEASE,
    [0xc3] = KEY_F9 | KEY_RELEASE,
    [0xc4] = KEY_F10 | KEY_RELEASE,
    [0xc5] = KEY_NUM_LOCK | KEY_RELEASE,
    [0xc6] = KEY_SCROLL_LOCK | KEY_RELEASE,
    [0xc7] = KEY_KEYPAD_7 | KEY_RELEASE,
    [0xc8] = KEY_KEYPAD_8 | KEY_RELEASE,
    [0xc9] = KEY_KEYPAD_9 | KEY_RELEASE,
    [0xca] = KEY_KEYPAD_DASH | KEY_RELEASE,
    [0xcb] = KEY_KEYPAD_4 | KEY_RELEASE,
    [0xcc] = KEY_KEYPAD_5 | KEY_RELEASE,
    [0xcd] = KEY_KEYPAD_6 | KEY_RELEASE,
    [0xce] = KEY_KEYPAD_PLUS | KEY_RELEASE,
    [0xcf] = KEY_KEYPAD_1 | KEY_RELEASE,
    [0xd0] = KEY_KEYPAD_2 | KEY_RELEASE,
    [0xd1] = KEY_KEYPAD_3 | KEY_RELEASE,
    [0xd2] = KEY_KEYPAD_0 | KEY_RELEASE,
    [0xd3] = KEY_KEYPAD_PERIOD | KEY_RELEASE,
    [0xd7] = KEY_F11 | KEY_RELEASE,
    [0xd8] = KEY_F12 | KEY_RELEASE,
};

static const uint32_t g_ps2_scancodes_fi[0xff] = {
    [0x02] = KEY_1 | KEY_PRESS,
    [0x03] = KEY_2 | KEY_PRESS,
    [0x04] = KEY_3 | KEY_PRESS,
    [0x05] = KEY_4 | KEY_PRESS,
    [0x06] = KEY_5 | KEY_PRESS,
    [0x07] = KEY_6 | KEY_PRESS,
    [0x08] = KEY_7 | KEY_PRESS,
    [0x09] = KEY_8 | KEY_PRESS,
    [0x0a] = KEY_9 | KEY_PRESS,
    [0x0b] = KEY_0 | KEY_PRESS,
    [0x0c] = KEY_PLUS | KEY_PRESS,
    [0x0d] = KEY_ACUTE_ACCENT | KEY_PRESS,
    [0x0e] = KEY_BACKSPACE | KEY_PRESS,
    [0x0f] = KEY_TAB | KEY_PRESS,
    [0x10] = KEY_Q | KEY_PRESS,
    [0x11] = KEY_W | KEY_PRESS,
    [0x12] = KEY_E | KEY_PRESS,
    [0x13] = KEY_R | KEY_PRESS,
    [0x14] = KEY_T | KEY_PRESS,
    [0x15] = KEY_Y | KEY_PRESS,
    [0x16] = KEY_U | KEY_PRESS,
    [0x17] = KEY_I | KEY_PRESS,
    [0x18] = KEY_O | KEY_PRESS,
    [0x19] = KEY_P | KEY_PRESS,
    [0x1a] = KEY_ARING | KEY_PRESS,
    [0x1b] = KEY_DIAERESIS | KEY_PRESS,
    [0x1c] = KEY_ENTER | KEY_PRESS,
    [0x1d] = KEY_CTRL_LEFT | KEY_PRESS,
    [0x1e] = KEY_A | KEY_PRESS,
    [0x1f] = KEY_S | KEY_PRESS,
    [0x20] = KEY_D | KEY_PRESS,
    [0x21] = KEY_F | KEY_PRESS,
    [0x22] = KEY_G | KEY_PRESS,
    [0x23] = KEY_H | KEY_PRESS,
    [0x24] = KEY_J | KEY_PRESS,
    [0x25] = KEY_K | KEY_PRESS,
    [0x26] = KEY_L | KEY_PRESS,
    [0x27] = KEY_O_DIAERESIS | KEY_PRESS,
    [0x28] = KEY_A_DIAERESIS | KEY_PRESS,
    [0x29] = KEY_SECTION | KEY_PRESS,
    [0x2a] = KEY_SHIFT_LEFT | KEY_PRESS,
    [0x2b] = KEY_SINGLE_QUOTE | KEY_PRESS,
    [0x2c] = KEY_Z | KEY_PRESS,
    [0x2d] = KEY_X | KEY_PRESS,
    [0x2e] = KEY_C | KEY_PRESS,
    [0x2f] = KEY_V | KEY_PRESS,
    [0x30] = KEY_B | KEY_PRESS,
    [0x31] = KEY_N | KEY_PRESS,
    [0x32] = KEY_M | KEY_PRESS,
    [0x33] = KEY_COMMA | KEY_PRESS,
    [0x34] = KEY_PERIOD | KEY_PRESS,
    [0x35] = KEY_DASH | KEY_PRESS,
    [0x36] = KEY_SHIFT_RIGHT | KEY_PRESS,
    [0x37] = KEY_KEYPAD_ASTERISK | KEY_PRESS,
    [0x38] = KEY_ALT_LEFT | KEY_PRESS,
    [0x39] = KEY_SPACE | KEY_PRESS,
    [0x3a] = KEY_CAPS_LOCK | KEY_PRESS,
    [0x3b] = KEY_F1 | KEY_PRESS,
    [0x3c] = KEY_F2 | KEY_PRESS,
    [0x3d] = KEY_F3 | KEY_PRESS,
    [0x3e] = KEY_F4 | KEY_PRESS,
    [0x3f] = KEY_F5 | KEY_PRESS,
    [0x40] = KEY_F6 | KEY_PRESS,
    [0x41] = KEY_F7 | KEY_PRESS,
    [0x42] = KEY_F8 | KEY_PRESS,
    [0x43] = KEY_F9 | KEY_PRESS,
    [0x44] = KEY_F10 | KEY_PRESS,
    [0x45] = KEY_NUM_LOCK | KEY_PRESS,
    [0x46] = KEY_SCROLL_LOCK | KEY_PRESS,
    [0x47] = KEY_KEYPAD_7 | KEY_PRESS,
    [0x48] = KEY_KEYPAD_8 | KEY_PRESS,
    [0x49] = KEY_KEYPAD_9 | KEY_PRESS,
    [0x4a] = KEY_KEYPAD_DASH | KEY_PRESS,
    [0x4b] = KEY_KEYPAD_4 | KEY_PRESS,
    [0x4c] = KEY_KEYPAD_5 | KEY_PRESS,
    [0x4d] = KEY_KEYPAD_6 | KEY_PRESS,
    [0x4e] = KEY_KEYPAD_PLUS | KEY_PRESS,
    [0x4f] = KEY_KEYPAD_1 | KEY_PRESS,
    [0x50] = KEY_KEYPAD_2 | KEY_PRESS,
    [0x51] = KEY_KEYPAD_3 | KEY_PRESS,
    [0x52] = KEY_KEYPAD_0 | KEY_PRESS,
    [0x53] = KEY_KEYPAD_PERIOD | KEY_PRESS,
    [0x56] = KEY_LT | KEY_PRESS,
    [0x57] = KEY_F11 | KEY_PRESS,
    [0x58] = KEY_F12 | KEY_PRESS,
    [0x82] = KEY_1 | KEY_RELEASE,
    [0x83] = KEY_2 | KEY_RELEASE,
    [0x84] = KEY_3 | KEY_RELEASE,
    [0x85] = KEY_4 | KEY_RELEASE,
    [0x86] = KEY_5 | KEY_RELEASE,
    [0x87] = KEY_6 | KEY_RELEASE,
    [0x88] = KEY_7 | KEY_RELEASE,
    [0x89] = KEY_8 | KEY_RELEASE,
    [0x8a] = KEY_9 | KEY_RELEASE,
    [0x8b] = KEY_0 | KEY_RELEASE,
    [0x8c] = KEY_PLUS | KEY_RELEASE,
    [0x8d] = KEY_ACUTE_ACCENT | KEY_RELEASE,
    [0x8e] = KEY_BACKSPACE | KEY_RELEASE,
    [0x8f] = KEY_TAB | KEY_RELEASE,
    [0x90] = KEY_Q | KEY_RELEASE,
    [0x91] = KEY_W | KEY_RELEASE,
    [0x92] = KEY_E | KEY_RELEASE,
    [0x93] = KEY_R | KEY_RELEASE,
    [0x94] = KEY_T | KEY_RELEASE,
    [0x95] = KEY_Y | KEY_RELEASE,
    [0x96] = KEY_U | KEY_RELEASE,
    [0x97] = KEY_I | KEY_RELEASE,
    [0x98] = KEY_O | KEY_RELEASE,
    [0x99] = KEY_P | KEY_RELEASE,
    [0x9a] = KEY_ARING | KEY_RELEASE,
    [0x9b] = KEY_DIAERESIS | KEY_RELEASE,
    [0x9c] = KEY_ENTER | KEY_RELEASE,
    [0x9d] = KEY_CTRL_LEFT | KEY_RELEASE,
    [0x9e] = KEY_A | KEY_RELEASE,
    [0x9f] = KEY_S | KEY_RELEASE,
    [0xa0] = KEY_D | KEY_RELEASE,
    [0xa1] = KEY_F | KEY_RELEASE,
    [0xa2] = KEY_G | KEY_RELEASE,
    [0xa3] = KEY_H | KEY_RELEASE,
    [0xa4] = KEY_J | KEY_RELEASE,
    [0xa5] = KEY_K | KEY_RELEASE,
    [0xa6] = KEY_L | KEY_RELEASE,
    [0xa7] = KEY_O_DIAERESIS | KEY_RELEASE,
    [0xa8] = KEY_A_DIAERESIS | KEY_RELEASE,
    [0xa9] = KEY_SECTION | KEY_RELEASE,
    [0xaa] = KEY_SHIFT_LEFT | KEY_RELEASE,
    [0xab] = KEY_SINGLE_QUOTE | KEY_RELEASE,
    [0xac] = KEY_Z | KEY_RELEASE,
    [0xad] = KEY_X | KEY_RELEASE,
    [0xae] = KEY_C | KEY_RELEASE,
    [0xaf] = KEY_V | KEY_RELEASE,
    [0xb0] = KEY_B | KEY_RELEASE,
    [0xb1] = KEY_N | KEY_RELEASE,
    [0xb2] = KEY_M | KEY_RELEASE,
    [0xb3] = KEY_COMMA | KEY_RELEASE,
    [0xb4] = KEY_PERIOD | KEY_RELEASE,
    [0xb5] = KEY_DASH | KEY_RELEASE,
    [0xb6] = KEY_SHIFT_RIGHT | KEY_RELEASE,
    [0xb7] = KEY_KEYPAD_ASTERISK | KEY_RELEASE,
    [0xb8] = KEY_ALT_LEFT | KEY_RELEASE,
    [0xb9] = KEY_SPACE | KEY_RELEASE,
    [0xba] = KEY_CAPS_LOCK | KEY_RELEASE,
    [0xbb] = KEY_F1 | KEY_RELEASE,
    [0xbc] = KEY_F2 | KEY_RELEASE,
    [0xbd] = KEY_F3 | KEY_RELEASE,
    [0xbe] = KEY_F4 | KEY_RELEASE,
    [0xbf] = KEY_F5 | KEY_RELEASE,
    [0xc0] = KEY_F6 | KEY_RELEASE,
    [0xc1] = KEY_F7 | KEY_RELEASE,
    [0xc2] = KEY_F8 | KEY_RELEASE,
    [0xc3] = KEY_F9 | KEY_RELEASE,
    [0xc4] = KEY_F10 | KEY_RELEASE,
    [0xc5] = KEY_NUM_LOCK | KEY_RELEASE,
    [0xc6] = KEY_SCROLL_LOCK | KEY_RELEASE,
    [0xc7] = KEY_KEYPAD_7 | KEY_RELEASE,
    [0xc8] = KEY_KEYPAD_8 | KEY_RELEASE,
    [0xc9] = KEY_KEYPAD_9 | KEY_RELEASE,
    [0xca] = KEY_KEYPAD_DASH | KEY_RELEASE,
    [0xcb] = KEY_KEYPAD_4 | KEY_RELEASE,
    [0xcc] = KEY_KEYPAD_5 | KEY_RELEASE,
    [0xcd] = KEY_KEYPAD_6 | KEY_RELEASE,
    [0xce] = KEY_KEYPAD_PLUS | KEY_RELEASE,
    [0xcf] = KEY_KEYPAD_1 | KEY_RELEASE,
    [0xd0] = KEY_KEYPAD_2 | KEY_RELEASE,
    [0xd1] = KEY_KEYPAD_3 | KEY_RELEASE,
    [0xd2] = KEY_KEYPAD_0 | KEY_RELEASE,
    [0xd3] = KEY_KEYPAD_PERIOD | KEY_RELEASE,
    [0xd6] = KEY_LT | KEY_RELEASE,
    [0xd7] = KEY_F11 | KEY_RELEASE,
    [0xd8] = KEY_F12 | KEY_RELEASE,
};

WCHAR keycode_to_char(enum keymap keys, uint32_t key, enum keyboard_modifiers modifiers) {
    key &= ~(KEY_RELEASE | KEY_KEYPAD);
    int flag_shift = ((modifiers & KB_MODIFIER_SHIFT) == KB_MODIFIER_SHIFT);
    if (flag_shift) {
        switch (keys) {
            case KEYMAP_US:
                switch (key) {
                    case KEY_1: return 0x2100;
                    case KEY_2: return 0x4000;
                    case KEY_3: return 0x2300;
                    case KEY_4: return 0x2400;
                    case KEY_5: return 0x2500;
                    case KEY_6: return 0x5e00;
                    case KEY_7: return 0x2600;
                    case KEY_8: return 0x2a00;
                    case KEY_9: return 0x2800;
                    case KEY_0: return 0x2900;
                    case KEY_DASH: return 0x5f00;
                    case KEY_EQUALS: return 0x2b00;
                    case KEY_SQUARE_BRACKET_OPEN: return 0x7b00;
                    case KEY_SQUARE_BRACKET_CLOSE: return 0x7d00;
                    case KEY_SEMICOLON: return 0x3a00;
                    case KEY_SINGLE_QUOTE: return 0x2200;
                    case KEY_BACKTICK: return 0x7e00;
                    case KEY_BACKSLASH: return 0x7c00;
                    case KEY_COMMA: return 0x3c00;
                    case KEY_PERIOD: return 0x3e00;
                    case KEY_SLASH: return 0x3f00;
                    default: break;
                }
                break;
            case KEYMAP_FI:
                switch (key) {
                    case KEY_1: return 0x2100;
                    case KEY_2: return 0x2200;
                    case KEY_3: return 0x2300;
                    case KEY_4: return 0xc2a4;
                    case KEY_5: return 0x2500;
                    case KEY_6: return 0x2600;
                    case KEY_7: return 0x2f00;
                    case KEY_8: return 0x2800;
                    case KEY_9: return 0x2900;
                    case KEY_0: return 0x3d00;
                    case KEY_PLUS: return 0x3f00;
                    case KEY_ACUTE_ACCENT: return 0x6000;
                    case KEY_DIAERESIS: return 0x5e00;
                    case KEY_SECTION: return 0xc2bd;
                    case KEY_SINGLE_QUOTE: return 0x2a00;
                    case KEY_COMMA: return 0x3b00;
                    case KEY_PERIOD: return 0x3a00;
                    case KEY_DASH: return 0x5f00;
                    case KEY_LT: return 0x3e00;
                    default: break;
                }
                break;
        }
    }
    int flag_capitalize = flag_shift ^ ((modifiers & KB_MODIFIER_CAPS_LOCK) == KB_MODIFIER_CAPS_LOCK);
    if (key >= 0x20 && key < 0xf0) {
        if (flag_capitalize)
            return 0xff00 & (to_upper(key) << 8);
        else
            return 0xff00 & (key << 8);
    }
    else if (is_utf8(key)) {
        if (flag_capitalize)
            return to_upper_utf8(key);
        else
            return key;
    }
    switch (key) {
        case KEY_ENTER:
            return 0x0a00;
        case KEY_TAB:
            return 0x0900;
        default:
            return 0x0000;
    }
}

const uint32_t* ps2_scancodes_get(enum keymap keys) {
    switch (keys) {
        case KEYMAP_US: return g_ps2_scancodes_us;
        case KEYMAP_FI: return g_ps2_scancodes_fi;
        default: return 0;
    }
}
