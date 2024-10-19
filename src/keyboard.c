#include "keyboard.h"
#include "io.h"
#include "irq.h"

char keycode_to_char(enum keycode key) {
    switch (key) {
        case KEY_0: case KEY_KEYPAD_0: return '0';
        case KEY_1: case KEY_KEYPAD_1: return '1';
        case KEY_2: case KEY_KEYPAD_2: return '2';
        case KEY_3: case KEY_KEYPAD_3: return '3';
        case KEY_4: case KEY_KEYPAD_4: return '4';
        case KEY_5: case KEY_KEYPAD_5: return '5';
        case KEY_6: case KEY_KEYPAD_6: return '6';
        case KEY_7: case KEY_KEYPAD_7: return '7';
        case KEY_8: case KEY_KEYPAD_8: return '8';
        case KEY_9: case KEY_KEYPAD_9: return '9';

        case KEY_A: return 'a';
        case KEY_B: return 'b';
        case KEY_C: return 'c';
        case KEY_D: return 'd';
        case KEY_E: return 'e';
        case KEY_F: return 'f';
        case KEY_G: return 'g';
        case KEY_H: return 'h';
        case KEY_I: return 'i';
        case KEY_J: return 'j';
        case KEY_K: return 'k';
        case KEY_L: return 'l';
        case KEY_M: return 'm';
        case KEY_N: return 'n';
        case KEY_O: return 'o';
        case KEY_P: return 'p';
        case KEY_Q: return 'q';
        case KEY_R: return 'r';
        case KEY_S: return 's';
        case KEY_T: return 't';
        case KEY_U: return 'u';
        case KEY_V: return 'v';
        case KEY_W: return 'w';
        case KEY_X: return 'x';
        case KEY_Y: return 'y';
        case KEY_Z: return 'z';

        case KEY_DASH: case KEY_KEYPAD_DASH: return '-';
        case KEY_EQUALS: return '=';
        case KEY_SPACE: return ' ';
        case KEY_TAB: return '\t';
        case KEY_SQUARE_BRACKET_OPEN: return '[';
        case KEY_SQUARE_BRACKET_CLOSE: return ']';
        case KEY_SEMICOLON: return ';';
        case KEY_SINGLE_QUOTE: return '\'';
        case KEY_BACKTICK: return '`';
        case KEY_BACKSLASH: return '\\';
        case KEY_COMMA: return ',';
        case KEY_PERIOD: case KEY_KEYPAD_PERIOD: return '.';
        case KEY_SLASH: return '/';
        case KEY_ENTER: return '\n';
        case KEY_KEYPAD_ASTERISK: return '*';
        case KEY_KEYPAD_PLUS: return '+';

        default: return '\0';
    }
}

int g_kb_caps_lock_on = 0;
int g_kb_shift_toggled = 0;
keyboard_event_handler g_kb_on_key_press = 0;
keyboard_event_handler g_kb_on_key_release = 0;

void kb_on_key_press(keyboard_event_handler handler) {
    g_kb_on_key_press = handler;
}
void kb_on_key_release(keyboard_event_handler handler) {
    g_kb_on_key_release = handler;
}

void key_press(enum keycode key) {
    if (g_kb_on_key_press != 0)
        g_kb_on_key_press(key);
}

void key_release(enum keycode key) {
    if (g_kb_on_key_release != 0)
        g_kb_on_key_release(key);
}

#define PS2_KEYBOARD_DATA_PORT 0x60
#define PS2_KEYBOARD_CMD_PORT 0x64

// https://wiki.osdev.org/PS/2_Keyboard
enum ps2_scancode {
    PS2_KEY_0_PRESSED = 0x0b,
    PS2_KEY_1_PRESSED = 0x02,
    PS2_KEY_2_PRESSED = 0x03,
    PS2_KEY_3_PRESSED = 0x04,
    PS2_KEY_4_PRESSED = 0x05,
    PS2_KEY_5_PRESSED = 0x06,
    PS2_KEY_6_PRESSED = 0x07,
    PS2_KEY_7_PRESSED = 0x08,
    PS2_KEY_8_PRESSED = 0x09,
    PS2_KEY_9_PRESSED = 0x0a,

    PS2_KEY_Q_PRESSED = 0x10,
    PS2_KEY_W_PRESSED = 0x11,
    PS2_KEY_E_PRESSED = 0x12,
    PS2_KEY_R_PRESSED = 0x13,
    PS2_KEY_T_PRESSED = 0x14,
    PS2_KEY_Y_PRESSED = 0x15,
    PS2_KEY_U_PRESSED = 0x16,
    PS2_KEY_I_PRESSED = 0x17,
    PS2_KEY_O_PRESSED = 0x18,
    PS2_KEY_P_PRESSED = 0x19,
    PS2_KEY_A_PRESSED = 0x1e,
    PS2_KEY_S_PRESSED = 0x1f,
    PS2_KEY_D_PRESSED = 0x20,
    PS2_KEY_F_PRESSED = 0x21,
    PS2_KEY_G_PRESSED = 0x22,
    PS2_KEY_H_PRESSED = 0x23,
    PS2_KEY_J_PRESSED = 0x24,
    PS2_KEY_K_PRESSED = 0x25,
    PS2_KEY_L_PRESSED = 0x26,
    PS2_KEY_Z_PRESSED = 0x2c,
    PS2_KEY_X_PRESSED = 0x2d,
    PS2_KEY_C_PRESSED = 0x2e,
    PS2_KEY_V_PRESSED = 0x2f,
    PS2_KEY_B_PRESSED = 0x30,
    PS2_KEY_N_PRESSED = 0x31,
    PS2_KEY_M_PRESSED = 0x32,

    PS2_KEY_F1_PRESSED = 0x3b,
    PS2_KEY_F2_PRESSED = 0x3c,
    PS2_KEY_F3_PRESSED = 0x3d,
    PS2_KEY_F4_PRESSED = 0x3e,
    PS2_KEY_F5_PRESSED = 0x3f,
    PS2_KEY_F6_PRESSED = 0x40,
    PS2_KEY_F7_PRESSED = 0x41,
    PS2_KEY_F8_PRESSED = 0x42,
    PS2_KEY_F9_PRESSED = 0x43,
    PS2_KEY_F10_PRESSED = 0x44,
    PS2_KEY_F11_PRESSED = 0x57,
    PS2_KEY_F12_PRESSED = 0x58,

    PS2_KEY_DASH_PRESSED = 0x0c,
    PS2_KEY_EQUALS_PRESSED = 0x0d,
    PS2_KEY_SPACE_PRESSED = 0x39,
    PS2_KEY_TAB_PRESSED = 0x0f,
    PS2_KEY_SQUARE_BRACKET_OPEN_PRESSED = 0x1a,
    PS2_KEY_SQUARE_BRACKET_CLOSE_PRESSED = 0x1b,
    PS2_KEY_SEMICOLON_PRESSED = 0x27,
    PS2_KEY_SINGLE_QUOTE_PRESSED = 0x28,
    PS2_KEY_BACKTICK_PRESSED = 0x29,
    PS2_KEY_BACKSLASH_PRESSED = 0x2b,
    PS2_KEY_COMMA_PRESSED = 0x33,
    PS2_KEY_PERIOD_PRESSED = 0x34,
    PS2_KEY_SLASH_PRESSED = 0x35,

    PS2_KEY_ESC_PRESSED = 0x01,
    PS2_KEY_BACKSPACE_PRESSED = 0x0e,
    PS2_KEY_ENTER_PRESSED = 0x1c,
    PS2_KEY_SHIFT_LEFT_PRESSED = 0x2a,
    PS2_KEY_SHIFT_RIGHT_PRESSED = 0x36,
    PS2_KEY_CTRL_LEFT_PRESSED = 0x1d,
    PS2_KEY_ALT_LEFT_PRESSED = 0x38,
    PS2_KEY_CAPS_LOCK_PRESSED = 0x3a,
    PS2_KEY_NUM_LOCK_PRESSED = 0x45,
    PS2_KEY_SCROLL_LOCK_PRESSED = 0x46,

    PS2_KEY_KEYPAD_0_PRESSED = 0x52,
    PS2_KEY_KEYPAD_1_PRESSED = 0x4f,
    PS2_KEY_KEYPAD_2_PRESSED = 0x50,
    PS2_KEY_KEYPAD_3_PRESSED = 0x51,
    PS2_KEY_KEYPAD_4_PRESSED = 0x4b,
    PS2_KEY_KEYPAD_5_PRESSED = 0x4c,
    PS2_KEY_KEYPAD_6_PRESSED = 0x4d,
    PS2_KEY_KEYPAD_7_PRESSED = 0x47,
    PS2_KEY_KEYPAD_8_PRESSED = 0x48,
    PS2_KEY_KEYPAD_9_PRESSED = 0x49,
    PS2_KEY_KEYPAD_ASTERISK_PRESSED = 0x37,
    PS2_KEY_KEYPAD_DASH_PRESSED = 0x4a,
    PS2_KEY_KEYPAD_PLUS_PRESSED = 0x4e,
    PS2_KEY_KEYPAD_PERIOD_PRESSED = 0x53,

    PS2_KEY_0_RELEASED = 0x8b,
    PS2_KEY_1_RELEASED = 0x82,
    PS2_KEY_2_RELEASED = 0x83,
    PS2_KEY_3_RELEASED = 0x84,
    PS2_KEY_4_RELEASED = 0x85,
    PS2_KEY_5_RELEASED = 0x86,
    PS2_KEY_6_RELEASED = 0x87,
    PS2_KEY_7_RELEASED = 0x88,
    PS2_KEY_8_RELEASED = 0x89,
    PS2_KEY_9_RELEASED = 0x8a,

    PS2_KEY_Q_RELEASED = 0x90,
    PS2_KEY_W_RELEASED = 0x91,
    PS2_KEY_E_RELEASED = 0x92,
    PS2_KEY_R_RELEASED = 0x93,
    PS2_KEY_T_RELEASED = 0x94,
    PS2_KEY_Y_RELEASED = 0x95,
    PS2_KEY_U_RELEASED = 0x96,
    PS2_KEY_I_RELEASED = 0x97,
    PS2_KEY_O_RELEASED = 0x98,
    PS2_KEY_P_RELEASED = 0x99,
    PS2_KEY_A_RELEASED = 0x9e,
    PS2_KEY_S_RELEASED = 0x9f,
    PS2_KEY_D_RELEASED = 0xa0,
    PS2_KEY_F_RELEASED = 0xa1,
    PS2_KEY_G_RELEASED = 0xa2,
    PS2_KEY_H_RELEASED = 0xa3,
    PS2_KEY_J_RELEASED = 0xa4,
    PS2_KEY_K_RELEASED = 0xa5,
    PS2_KEY_L_RELEASED = 0xa6,
    PS2_KEY_Z_RELEASED = 0xac,
    PS2_KEY_X_RELEASED = 0xad,
    PS2_KEY_C_RELEASED = 0xae,
    PS2_KEY_V_RELEASED = 0xaf,
    PS2_KEY_B_RELEASED = 0xb0,
    PS2_KEY_N_RELEASED = 0xb1,
    PS2_KEY_M_RELEASED = 0xb2,

    PS2_KEY_F1_RELEASED = 0xbb,
    PS2_KEY_F2_RELEASED = 0xbc,
    PS2_KEY_F3_RELEASED = 0xbd,
    PS2_KEY_F4_RELEASED = 0xbe,
    PS2_KEY_F5_RELEASED = 0xbf,
    PS2_KEY_F6_RELEASED = 0xc0,
    PS2_KEY_F7_RELEASED = 0xc1,
    PS2_KEY_F8_RELEASED = 0xc2,
    PS2_KEY_F9_RELEASED = 0xc3,
    PS2_KEY_F10_RELEASED = 0xc4,
    PS2_KEY_F11_RELEASED = 0xd7,
    PS2_KEY_F12_RELEASED = 0xd8,

    PS2_KEY_DASH_RELEASED = 0x8c,
    PS2_KEY_EQUALS_RELEASED = 0x8d,
    PS2_KEY_SPACE_RELEASED = 0xb9,
    PS2_KEY_TAB_RELEASED = 0x8f,
    PS2_KEY_SQUARE_BRACKET_OPEN_RELEASED = 0x9a,
    PS2_KEY_SQUARE_BRACKET_CLOSE_RELEASED = 0x9b,
    PS2_KEY_SEMICOLON_RELEASED = 0xa7,
    PS2_KEY_SINGLE_QUOTE_RELEASED = 0xa8,
    PS2_KEY_BACKTICK_RELEASED = 0xa9,
    PS2_KEY_BACKSLASH_RELEASED = 0xab,
    PS2_KEY_COMMA_RELEASED = 0xb3,
    PS2_KEY_PERIOD_RELEASED = 0xb4,
    PS2_KEY_SLASH_RELEASED = 0xb5,

    PS2_KEY_ESC_RELEASED = 0x81,
    PS2_KEY_BACKSPACE_RELEASED = 0x8e,
    PS2_KEY_ENTER_RELEASED = 0x9c,
    PS2_KEY_SHIFT_LEFT_RELEASED = 0xaa,
    PS2_KEY_SHIFT_RIGHT_RELEASED = 0xb6,
    PS2_KEY_CTRL_LEFT_RELEASED = 0x9d,
    PS2_KEY_ALT_LEFT_RELEASED = 0xb8,
    PS2_KEY_CAPS_LOCK_RELEASED = 0xba,
    PS2_KEY_NUM_LOCK_RELEASED = 0xc5,
    PS2_KEY_SCROLL_LOCK_RELEASED = 0xc6,

    PS2_KEY_KEYPAD_0_RELEASED = 0xd2,
    PS2_KEY_KEYPAD_1_RELEASED = 0xcf,
    PS2_KEY_KEYPAD_2_RELEASED = 0xd0,
    PS2_KEY_KEYPAD_3_RELEASED = 0xd1,
    PS2_KEY_KEYPAD_4_RELEASED = 0xcb,
    PS2_KEY_KEYPAD_5_RELEASED = 0xcc,
    PS2_KEY_KEYPAD_6_RELEASED = 0xcd,
    PS2_KEY_KEYPAD_7_RELEASED = 0xc7,
    PS2_KEY_KEYPAD_8_RELEASED = 0xc8,
    PS2_KEY_KEYPAD_9_RELEASED = 0xc9,
    PS2_KEY_KEYPAD_ASTERISK_RELEASED = 0xb7,
    PS2_KEY_KEYPAD_DASH_RELEASED = 0xca,
    PS2_KEY_KEYPAD_PLUS_RELEASED = 0xce,
    PS2_KEY_KEYPAD_PERIOD_RELEASED = 0xd3,
};

void handle_keyboard() {
    uint8_t scancode = inb(PS2_KEYBOARD_DATA_PORT);
    switch (scancode) {
        case PS2_KEY_0_PRESSED: key_press(KEY_0); break;
        case PS2_KEY_1_PRESSED: key_press(KEY_1); break;
        case PS2_KEY_2_PRESSED: key_press(KEY_2); break;
        case PS2_KEY_3_PRESSED: key_press(KEY_3); break;
        case PS2_KEY_4_PRESSED: key_press(KEY_4); break;
        case PS2_KEY_5_PRESSED: key_press(KEY_5); break;
        case PS2_KEY_6_PRESSED: key_press(KEY_6); break;
        case PS2_KEY_7_PRESSED: key_press(KEY_7); break;
        case PS2_KEY_8_PRESSED: key_press(KEY_8); break;
        case PS2_KEY_9_PRESSED: key_press(KEY_9); break;

        case PS2_KEY_Q_PRESSED: key_press(KEY_Q); break;
        case PS2_KEY_W_PRESSED: key_press(KEY_W); break;
        case PS2_KEY_E_PRESSED: key_press(KEY_E); break;
        case PS2_KEY_R_PRESSED: key_press(KEY_R); break;
        case PS2_KEY_T_PRESSED: key_press(KEY_T); break;
        case PS2_KEY_Y_PRESSED: key_press(KEY_Y); break;
        case PS2_KEY_U_PRESSED: key_press(KEY_U); break;
        case PS2_KEY_I_PRESSED: key_press(KEY_I); break;
        case PS2_KEY_O_PRESSED: key_press(KEY_O); break;
        case PS2_KEY_P_PRESSED: key_press(KEY_P); break;
        case PS2_KEY_A_PRESSED: key_press(KEY_A); break;
        case PS2_KEY_S_PRESSED: key_press(KEY_S); break;
        case PS2_KEY_D_PRESSED: key_press(KEY_D); break;
        case PS2_KEY_F_PRESSED: key_press(KEY_F); break;
        case PS2_KEY_G_PRESSED: key_press(KEY_G); break;
        case PS2_KEY_H_PRESSED: key_press(KEY_H); break;
        case PS2_KEY_J_PRESSED: key_press(KEY_J); break;
        case PS2_KEY_K_PRESSED: key_press(KEY_K); break;
        case PS2_KEY_L_PRESSED: key_press(KEY_L); break;
        case PS2_KEY_Z_PRESSED: key_press(KEY_Z); break;
        case PS2_KEY_X_PRESSED: key_press(KEY_X); break;
        case PS2_KEY_C_PRESSED: key_press(KEY_C); break;
        case PS2_KEY_V_PRESSED: key_press(KEY_V); break;
        case PS2_KEY_B_PRESSED: key_press(KEY_B); break;
        case PS2_KEY_N_PRESSED: key_press(KEY_N); break;
        case PS2_KEY_M_PRESSED: key_press(KEY_M); break;

        case PS2_KEY_F1_PRESSED: key_press(KEY_F1); break;
        case PS2_KEY_F2_PRESSED: key_press(KEY_F2); break;
        case PS2_KEY_F3_PRESSED: key_press(KEY_F3); break;
        case PS2_KEY_F4_PRESSED: key_press(KEY_F4); break;
        case PS2_KEY_F5_PRESSED: key_press(KEY_F5); break;
        case PS2_KEY_F6_PRESSED: key_press(KEY_F6); break;
        case PS2_KEY_F7_PRESSED: key_press(KEY_F7); break;
        case PS2_KEY_F8_PRESSED: key_press(KEY_F8); break;
        case PS2_KEY_F9_PRESSED: key_press(KEY_F9); break;
        case PS2_KEY_F10_PRESSED: key_press(KEY_F10); break;
        case PS2_KEY_F11_PRESSED: key_press(KEY_F11); break;
        case PS2_KEY_F12_PRESSED: key_press(KEY_F12); break;

        case PS2_KEY_DASH_PRESSED: key_press(KEY_DASH); break;
        case PS2_KEY_EQUALS_PRESSED: key_press(KEY_EQUALS); break;
        case PS2_KEY_SPACE_PRESSED: key_press(KEY_SPACE); break;
        case PS2_KEY_TAB_PRESSED: key_press(KEY_TAB); break;
        case PS2_KEY_SQUARE_BRACKET_OPEN_PRESSED: key_press(KEY_SQUARE_BRACKET_OPEN); break;
        case PS2_KEY_SQUARE_BRACKET_CLOSE_PRESSED: key_press(KEY_SQUARE_BRACKET_CLOSE); break;
        case PS2_KEY_SEMICOLON_PRESSED: key_press(KEY_SEMICOLON); break;
        case PS2_KEY_SINGLE_QUOTE_PRESSED: key_press(KEY_SINGLE_QUOTE); break;
        case PS2_KEY_BACKTICK_PRESSED: key_press(KEY_BACKTICK); break;
        case PS2_KEY_BACKSLASH_PRESSED: key_press(KEY_BACKSLASH); break;
        case PS2_KEY_COMMA_PRESSED: key_press(KEY_COMMA); break;
        case PS2_KEY_PERIOD_PRESSED: key_press(KEY_PERIOD); break;
        case PS2_KEY_SLASH_PRESSED: key_press(KEY_SLASH); break;

        case PS2_KEY_ESC_PRESSED: key_press(KEY_ESC); break;
        case PS2_KEY_BACKSPACE_PRESSED: key_press(KEY_BACKSPACE); break;
        case PS2_KEY_ENTER_PRESSED: key_press(KEY_ENTER); break;
        case PS2_KEY_SHIFT_LEFT_PRESSED: key_press(KEY_SHIFT_LEFT); break;
        case PS2_KEY_SHIFT_RIGHT_PRESSED: key_press(KEY_SHIFT_RIGHT); break;
        case PS2_KEY_CTRL_LEFT_PRESSED: key_press(KEY_CTRL_LEFT); break;
        case PS2_KEY_ALT_LEFT_PRESSED: key_press(KEY_ALT_LEFT); break;
        case PS2_KEY_CAPS_LOCK_PRESSED: key_press(KEY_CAPS_LOCK); break;
        case PS2_KEY_NUM_LOCK_PRESSED: key_press(KEY_NUM_LOCK); break;
        case PS2_KEY_SCROLL_LOCK_PRESSED: key_press(KEY_SCROLL_LOCK); break;

        case PS2_KEY_KEYPAD_0_PRESSED: key_press(KEY_KEYPAD_0); break;
        case PS2_KEY_KEYPAD_1_PRESSED: key_press(KEY_KEYPAD_1); break;
        case PS2_KEY_KEYPAD_2_PRESSED: key_press(KEY_KEYPAD_2); break;
        case PS2_KEY_KEYPAD_3_PRESSED: key_press(KEY_KEYPAD_3); break;
        case PS2_KEY_KEYPAD_4_PRESSED: key_press(KEY_KEYPAD_4); break;
        case PS2_KEY_KEYPAD_5_PRESSED: key_press(KEY_KEYPAD_5); break;
        case PS2_KEY_KEYPAD_6_PRESSED: key_press(KEY_KEYPAD_6); break;
        case PS2_KEY_KEYPAD_7_PRESSED: key_press(KEY_KEYPAD_7); break;
        case PS2_KEY_KEYPAD_8_PRESSED: key_press(KEY_KEYPAD_8); break;
        case PS2_KEY_KEYPAD_9_PRESSED: key_press(KEY_KEYPAD_9); break;
        case PS2_KEY_KEYPAD_ASTERISK_PRESSED: key_press(KEY_KEYPAD_ASTERISK); break;
        case PS2_KEY_KEYPAD_DASH_PRESSED: key_press(KEY_KEYPAD_DASH); break;
        case PS2_KEY_KEYPAD_PLUS_PRESSED: key_press(KEY_KEYPAD_PLUS); break;
        case PS2_KEY_KEYPAD_PERIOD_PRESSED: key_press(KEY_KEYPAD_PERIOD); break;

        case PS2_KEY_0_RELEASED: key_release(KEY_0); break;
        case PS2_KEY_1_RELEASED: key_release(KEY_1); break;
        case PS2_KEY_2_RELEASED: key_release(KEY_2); break;
        case PS2_KEY_3_RELEASED: key_release(KEY_3); break;
        case PS2_KEY_4_RELEASED: key_release(KEY_4); break;
        case PS2_KEY_5_RELEASED: key_release(KEY_5); break;
        case PS2_KEY_6_RELEASED: key_release(KEY_6); break;
        case PS2_KEY_7_RELEASED: key_release(KEY_7); break;
        case PS2_KEY_8_RELEASED: key_release(KEY_8); break;
        case PS2_KEY_9_RELEASED: key_release(KEY_9); break;

        case PS2_KEY_Q_RELEASED: key_release(KEY_Q); break;
        case PS2_KEY_W_RELEASED: key_release(KEY_W); break;
        case PS2_KEY_E_RELEASED: key_release(KEY_E); break;
        case PS2_KEY_R_RELEASED: key_release(KEY_R); break;
        case PS2_KEY_T_RELEASED: key_release(KEY_T); break;
        case PS2_KEY_Y_RELEASED: key_release(KEY_Y); break;
        case PS2_KEY_U_RELEASED: key_release(KEY_U); break;
        case PS2_KEY_I_RELEASED: key_release(KEY_I); break;
        case PS2_KEY_O_RELEASED: key_release(KEY_O); break;
        case PS2_KEY_P_RELEASED: key_release(KEY_P); break;
        case PS2_KEY_A_RELEASED: key_release(KEY_A); break;
        case PS2_KEY_S_RELEASED: key_release(KEY_S); break;
        case PS2_KEY_D_RELEASED: key_release(KEY_D); break;
        case PS2_KEY_F_RELEASED: key_release(KEY_F); break;
        case PS2_KEY_G_RELEASED: key_release(KEY_G); break;
        case PS2_KEY_H_RELEASED: key_release(KEY_H); break;
        case PS2_KEY_J_RELEASED: key_release(KEY_J); break;
        case PS2_KEY_K_RELEASED: key_release(KEY_K); break;
        case PS2_KEY_L_RELEASED: key_release(KEY_L); break;
        case PS2_KEY_Z_RELEASED: key_release(KEY_Z); break;
        case PS2_KEY_X_RELEASED: key_release(KEY_X); break;
        case PS2_KEY_C_RELEASED: key_release(KEY_C); break;
        case PS2_KEY_V_RELEASED: key_release(KEY_V); break;
        case PS2_KEY_B_RELEASED: key_release(KEY_B); break;
        case PS2_KEY_N_RELEASED: key_release(KEY_N); break;
        case PS2_KEY_M_RELEASED: key_release(KEY_M); break;

        case PS2_KEY_F1_RELEASED: key_release(KEY_F1); break;
        case PS2_KEY_F2_RELEASED: key_release(KEY_F2); break;
        case PS2_KEY_F3_RELEASED: key_release(KEY_F3); break;
        case PS2_KEY_F4_RELEASED: key_release(KEY_F4); break;
        case PS2_KEY_F5_RELEASED: key_release(KEY_F5); break;
        case PS2_KEY_F6_RELEASED: key_release(KEY_F6); break;
        case PS2_KEY_F7_RELEASED: key_release(KEY_F7); break;
        case PS2_KEY_F8_RELEASED: key_release(KEY_F8); break;
        case PS2_KEY_F9_RELEASED: key_release(KEY_F9); break;
        case PS2_KEY_F10_RELEASED: key_release(KEY_F10); break;
        case PS2_KEY_F11_RELEASED: key_release(KEY_F11); break;
        case PS2_KEY_F12_RELEASED: key_release(KEY_F12); break;

        case PS2_KEY_DASH_RELEASED: key_release(KEY_DASH); break;
        case PS2_KEY_EQUALS_RELEASED: key_release(KEY_EQUALS); break;
        case PS2_KEY_SPACE_RELEASED: key_release(KEY_SPACE); break;
        case PS2_KEY_TAB_RELEASED: key_release(KEY_TAB); break;
        case PS2_KEY_SQUARE_BRACKET_OPEN_RELEASED: key_release(KEY_SQUARE_BRACKET_OPEN); break;
        case PS2_KEY_SQUARE_BRACKET_CLOSE_RELEASED: key_release(KEY_SQUARE_BRACKET_CLOSE); break;
        case PS2_KEY_SEMICOLON_RELEASED: key_release(KEY_SEMICOLON); break;
        case PS2_KEY_SINGLE_QUOTE_RELEASED: key_release(KEY_SINGLE_QUOTE); break;
        case PS2_KEY_BACKTICK_RELEASED: key_release(KEY_BACKTICK); break;
        case PS2_KEY_BACKSLASH_RELEASED: key_release(KEY_BACKSLASH); break;
        case PS2_KEY_COMMA_RELEASED: key_release(KEY_COMMA); break;
        case PS2_KEY_PERIOD_RELEASED: key_release(KEY_PERIOD); break;
        case PS2_KEY_SLASH_RELEASED: key_release(KEY_SLASH); break;

        case PS2_KEY_ESC_RELEASED: key_release(KEY_ESC); break;
        case PS2_KEY_BACKSPACE_RELEASED: key_release(KEY_BACKSPACE); break;
        case PS2_KEY_ENTER_RELEASED: key_release(KEY_ENTER); break;
        case PS2_KEY_SHIFT_LEFT_RELEASED: key_release(KEY_SHIFT_LEFT); break;
        case PS2_KEY_SHIFT_RIGHT_RELEASED: key_release(KEY_SHIFT_RIGHT); break;
        case PS2_KEY_CTRL_LEFT_RELEASED: key_release(KEY_CTRL_LEFT); break;
        case PS2_KEY_ALT_LEFT_RELEASED: key_release(KEY_ALT_LEFT); break;
        case PS2_KEY_CAPS_LOCK_RELEASED: key_release(KEY_CAPS_LOCK); break;
        case PS2_KEY_NUM_LOCK_RELEASED: key_release(KEY_NUM_LOCK); break;
        case PS2_KEY_SCROLL_LOCK_RELEASED: key_release(KEY_SCROLL_LOCK); break;

        case PS2_KEY_KEYPAD_0_RELEASED: key_release(KEY_KEYPAD_0); break;
        case PS2_KEY_KEYPAD_1_RELEASED: key_release(KEY_KEYPAD_1); break;
        case PS2_KEY_KEYPAD_2_RELEASED: key_release(KEY_KEYPAD_2); break;
        case PS2_KEY_KEYPAD_3_RELEASED: key_release(KEY_KEYPAD_3); break;
        case PS2_KEY_KEYPAD_4_RELEASED: key_release(KEY_KEYPAD_4); break;
        case PS2_KEY_KEYPAD_5_RELEASED: key_release(KEY_KEYPAD_5); break;
        case PS2_KEY_KEYPAD_6_RELEASED: key_release(KEY_KEYPAD_6); break;
        case PS2_KEY_KEYPAD_7_RELEASED: key_release(KEY_KEYPAD_7); break;
        case PS2_KEY_KEYPAD_8_RELEASED: key_release(KEY_KEYPAD_8); break;
        case PS2_KEY_KEYPAD_9_RELEASED: key_release(KEY_KEYPAD_9); break;
        case PS2_KEY_KEYPAD_ASTERISK_RELEASED: key_release(KEY_KEYPAD_ASTERISK); break;
        case PS2_KEY_KEYPAD_DASH_RELEASED: key_release(KEY_KEYPAD_DASH); break;
        case PS2_KEY_KEYPAD_PLUS_RELEASED: key_release(KEY_KEYPAD_PLUS); break;
        case PS2_KEY_KEYPAD_PERIOD_RELEASED: key_release(KEY_KEYPAD_PERIOD); break;

        default: break;
    }
}

void kb_init() {
    irq_assign_handler(1, handle_keyboard);

    cli();
    // poll all pending
    while (inb(PS2_KEYBOARD_CMD_PORT) & 1)
        inb(PS2_KEYBOARD_DATA_PORT);
    // activate port
    outb(PS2_KEYBOARD_CMD_PORT, 0xae);
    // get current state
    outb(PS2_KEYBOARD_CMD_PORT, 0x20);
    uint8_t status = (inb(PS2_KEYBOARD_DATA_PORT) | 1) & ~0x10; // ???
    // set state
    outb(PS2_KEYBOARD_CMD_PORT, 0x60);
    outb(PS2_KEYBOARD_DATA_PORT, status);

    outb(PS2_KEYBOARD_DATA_PORT, 0xf4);
    sti();
}
