#ifndef KEYBOARD_H
#define KEYBOARD_H

#include "types.h"

enum keycode {
    KEY_0, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, KEY_7, KEY_8, KEY_9,

    KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P,
    KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, KEY_H, KEY_J, KEY_K, KEY_L, KEY_Z,
    KEY_X, KEY_C, KEY_V, KEY_B, KEY_N, KEY_M,

    KEY_F1, KEY_F2, KEY_F3, KEY_F4, KEY_F5, KEY_F6, KEY_F7, KEY_F8, KEY_F9,
    KEY_F10, KEY_F11, KEY_F12,

    KEY_DASH,
    KEY_EQUALS,
    KEY_SPACE,
    KEY_TAB,
    KEY_SQUARE_BRACKET_OPEN, KEY_SQUARE_BRACKET_CLOSE,
    KEY_SEMICOLON,
    KEY_SINGLE_QUOTE,
    KEY_BACKTICK,
    KEY_BACKSLASH,
    KEY_COMMA,
    KEY_PERIOD,
    KEY_SLASH,

    KEY_ESC,
    KEY_BACKSPACE,
    KEY_ENTER,
    KEY_SHIFT_LEFT, KEY_SHIFT_RIGHT,
    KEY_CTRL_LEFT,
    KEY_ALT_LEFT,
    KEY_CAPS_LOCK,
    KEY_NUM_LOCK,
    KEY_SCROLL_LOCK,

    KEY_KEYPAD_0, KEY_KEYPAD_1, KEY_KEYPAD_2, KEY_KEYPAD_3, KEY_KEYPAD_4,
    KEY_KEYPAD_5, KEY_KEYPAD_6, KEY_KEYPAD_7, KEY_KEYPAD_8, KEY_KEYPAD_9,
    KEY_KEYPAD_ASTERISK,
    KEY_KEYPAD_DASH,
    KEY_KEYPAD_PLUS,
    KEY_KEYPAD_PERIOD,
};

char keycode_to_char(enum keycode key);
void kb_init();

typedef void (*keyboard_event_handler)(enum keycode key);
void kb_on_key_press(keyboard_event_handler handler);
void kb_on_key_release(keyboard_event_handler handler);

#endif
