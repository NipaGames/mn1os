#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"
#include "utf8.h"

enum keyboard_modifiers {
    KB_MODIFIER_NONE            = 0b00000000,
    KB_MODIFIER_SCROLL_LOCK     = 0b00000001,
    KB_MODIFIER_NUM_LOCK        = 0b00000010,
    KB_MODIFIER_CAPS_LOCK       = 0b00000100,
    KB_MODIFIER_SHIFT           = 0b00001000,
    KB_MODIFIER_CTRL            = 0b00010000,
    KB_MODIFIER_ALT             = 0b00100000,
    KB_MODIFIER_ALT_GR          = 0b01000000,
};

enum keycode {
    KEY_UNKNOWN = 0x00000000,
    KEY_PRESS   = 0x00000000,
    KEY_RELEASE = 0x10000000,
    KEY_KEYPAD  = 0x01000000,

    KEY_SPACE = 0x20,
    KEY_SINGLE_QUOTE = 0x27,
    KEY_PLUS = 0x2b,
    KEY_COMMA = 0x2c,
    KEY_DASH = 0x2d,
    KEY_PERIOD = 0x2e,
    KEY_SLASH = 0x2f,
    KEY_0 = 0x30,
    KEY_1 = 0x31,
    KEY_2 = 0x32,
    KEY_3 = 0x33,
    KEY_4 = 0x34,
    KEY_5 = 0x35,
    KEY_6 = 0x36,
    KEY_7 = 0x37,
    KEY_8 = 0x38,
    KEY_9 = 0x39,
    KEY_SEMICOLON = 0x3b,
    KEY_LT = 0x3c,
    KEY_EQUALS = 0x3d,
    KEY_A = 0x61,
    KEY_B = 0x62,
    KEY_C = 0x63,
    KEY_D = 0x64,
    KEY_E = 0x65,
    KEY_F = 0x66,
    KEY_G = 0x67,
    KEY_H = 0x68,
    KEY_I = 0x69,
    KEY_J = 0x6a,
    KEY_K = 0x6b,
    KEY_L = 0x6c,
    KEY_M = 0x6d,
    KEY_N = 0x6e,
    KEY_O = 0x6f,
    KEY_P = 0x70,
    KEY_Q = 0x71,
    KEY_R = 0x72,
    KEY_S = 0x73,
    KEY_T = 0x74,
    KEY_U = 0x75,
    KEY_V = 0x76,
    KEY_W = 0x77,
    KEY_X = 0x78,
    KEY_Y = 0x79,
    KEY_Z = 0x7a,
    KEY_SQUARE_BRACKET_OPEN = 0x5b,
    KEY_BACKSLASH = 0x5c,
    KEY_SQUARE_BRACKET_CLOSE = 0x5d,
    KEY_CIRCUMFLEX = 0x5e,
    KEY_BACKTICK = 0x60,
    KEY_SECTION = 0xc2a7,
    KEY_DIAERESIS = 0xc2a8,
    KEY_ACUTE_ACCENT = 0xc2b4,
    KEY_A_DIAERESIS = 0xc3a4,
    KEY_ARING = 0xc3a5,
    KEY_O_DIAERESIS = 0xc3b6,
    KEY_ESC         = 0x00010000,
    KEY_ENTER       = 0x00020000,
    KEY_TAB         = 0x00030000,
    KEY_BACKSPACE   = 0x00040000,
    KEY_SHIFT_LEFT  = 0x00050000,
    KEY_SHIFT_RIGHT = 0x00060000,
    KEY_CTRL_LEFT   = 0x00070000,
    KEY_ALT_LEFT    = 0x00080000,
    KEY_CAPS_LOCK   = 0x00090000,
    KEY_NUM_LOCK    = 0x000a0000,
    KEY_SCROLL_LOCK = 0x000b0000,
    KEY_ALT_GR      = 0x000c0000,
    KEY_LEFT        = 0x000d0000,
    KEY_DOWN        = 0x000e0000,
    KEY_UP          = 0x000f0000,
    KEY_RIGHT       = 0x00100000,
    KEY_PRT_SC      = 0x00110000,
    KEY_PAUSE       = 0x00120000,
    KEY_INSERT      = 0x00130000,
    KEY_HOME        = 0x00140000,
    KEY_DELETE      = 0x00150000,
    KEY_END         = 0x00160000,
    KEY_PAGE_UP     = 0x00170000,
    KEY_PAGE_DOWN   = 0x00180000,
    KEY_CTRL_RIGHT  = 0x00190000,
    KEY_F1          = 0x00800000,
    KEY_F2          = 0x00810000,
    KEY_F3          = 0x00820000,
    KEY_F4          = 0x00830000,
    KEY_F5          = 0x00840000,
    KEY_F6          = 0x00850000,
    KEY_F7          = 0x00860000,
    KEY_F8          = 0x00870000,
    KEY_F9          = 0x00880000,
    KEY_F10         = 0x00890000,
    KEY_F11         = 0x008a0000,
    KEY_F12         = 0x008b0000,
    KEY_KEYPAD_0 = KEY_KEYPAD | KEY_0,
    KEY_KEYPAD_1 = KEY_KEYPAD | KEY_1,
    KEY_KEYPAD_2 = KEY_KEYPAD | KEY_2,
    KEY_KEYPAD_3 = KEY_KEYPAD | KEY_3,
    KEY_KEYPAD_4 = KEY_KEYPAD | KEY_4,
    KEY_KEYPAD_5 = KEY_KEYPAD | KEY_5,
    KEY_KEYPAD_6 = KEY_KEYPAD | KEY_6,
    KEY_KEYPAD_7 = KEY_KEYPAD | KEY_7,
    KEY_KEYPAD_8 = KEY_KEYPAD | KEY_8,
    KEY_KEYPAD_9 = KEY_KEYPAD | KEY_9,
    KEY_KEYPAD_DASH = KEY_KEYPAD | KEY_DASH,
    KEY_KEYPAD_PERIOD = KEY_KEYPAD | KEY_PERIOD,
    KEY_KEYPAD_ASTERISK = KEY_KEYPAD | 0x2a,
    KEY_KEYPAD_PLUS = KEY_KEYPAD | 0x2b,
    KEY_KEYPAD_ENTER = KEY_KEYPAD | KEY_ENTER,
    KEY_KEYPAD_SLASH = KEY_KEYPAD | KEY_SLASH,
};

void kb_init();

typedef void (*kb_key_event_handler)(uint32_t key, WCHAR c);
void kb_on_key_press(kb_key_event_handler handler);
void kb_on_key_release(kb_key_event_handler handler);
enum keymap;
void kb_set_keymap(enum keymap keys);
enum keymap kb_get_keymap();

#endif
