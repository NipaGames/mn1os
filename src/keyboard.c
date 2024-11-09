#include "keyboard.h"
#include "io.h"
#include "irq.h"
#include "utf8.h"
#include "keymaps.h"

enum keyboard_modifiers g_kb_modifiers = KB_MODIFIER_NONE;
kb_key_event_handler g_kb_on_key_press = 0;
kb_key_event_handler g_kb_on_key_release = 0;
enum keymap g_kb_keymap = KEYMAP_US;
int g_kb_shift_left_down = 0;
int g_kb_shift_right_down = 0;
int g_kb_caps_lock_down = 0;

void kb_on_key_press(kb_key_event_handler handler) {
    g_kb_on_key_press = handler;
}
void kb_on_key_release(kb_key_event_handler handler) {
    g_kb_on_key_release = handler;
}
void kb_set_keymap(enum keymap keys) {
    g_kb_keymap = keys;
}
enum keymap kb_get_keymap() {
    return g_kb_keymap;
}

#include "terminal.h"
void key_press(enum keycode key) {
    if (g_kb_on_key_press == KEY_UNKNOWN)
        return;
    // disable repeats for these
    switch (key) {
        case KEY_SHIFT_LEFT:
            if (g_kb_shift_left_down)
                return;
            g_kb_shift_left_down = 1;
            break;
        case KEY_SHIFT_RIGHT:
            if (g_kb_shift_right_down)
                return;
            g_kb_shift_right_down = 1;
            break;
        case KEY_CAPS_LOCK:
            if (g_kb_caps_lock_down)
                return;
            g_kb_caps_lock_down = 1;
            break;
        default:
            break;
    }
    switch (key) {
        case KEY_SHIFT_LEFT:
        case KEY_SHIFT_RIGHT:
            g_kb_modifiers |= KB_MODIFIER_SHIFT;
            break;
        case KEY_CAPS_LOCK:
            g_kb_modifiers ^= KB_MODIFIER_CAPS_LOCK;
            break;
        default:
            break;
    }
    g_kb_on_key_press(key, keycode_to_char(g_kb_keymap, key, g_kb_modifiers));
}

void key_release(enum keycode key) {
    if (g_kb_on_key_release == KEY_UNKNOWN)
        return;
    switch (key) {
        case KEY_SHIFT_LEFT:
            g_kb_shift_left_down = 0;
            break;
        case KEY_SHIFT_RIGHT:
            g_kb_shift_right_down = 0;
            break;
        case KEY_CAPS_LOCK:
            g_kb_caps_lock_down = 0;
            break;
        default:
            break;
    }
    switch (key) {
        case KEY_SHIFT_LEFT:
        case KEY_SHIFT_RIGHT:
            if (!g_kb_shift_left_down && !g_kb_shift_right_down)
                g_kb_modifiers &= ~KB_MODIFIER_SHIFT;
            break;
        default:
            break;
    }
    g_kb_on_key_release(key, keycode_to_char(g_kb_keymap, key, g_kb_modifiers));
}

#define PS2_KEYBOARD_DATA_PORT 0x60
#define PS2_KEYBOARD_CMD_PORT 0x64

void handle_keyboard() {
    uint8_t raw = inb(PS2_KEYBOARD_DATA_PORT);
    uint32_t keycode = ps2_scancodes_get(g_kb_keymap)[raw];
    if (keycode == 0)
        return;
    if ((keycode & KEY_RELEASE) == KEY_RELEASE)
        key_release(keycode & ~KEY_RELEASE);
    else
        key_press(keycode);
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
    // enable scancodes
    outb(PS2_KEYBOARD_DATA_PORT, 0xf4);
    sti();
}
