#include "keyboard.h"
#include "io.h"
#include "irq.h"
#include "utf8.h"
#include "keymaps.h"

#define PS2_KEYBOARD_DATA_PORT 0x60
#define PS2_KEYBOARD_CMD_PORT 0x64

enum keyboard_modifiers g_kb_modifiers;
kb_key_event_handler g_kb_on_key_press;
kb_key_event_handler g_kb_on_key_release;
enum keymap g_kb_keymap;

volatile int g_kb_shift_left_down;
volatile int g_kb_shift_right_down;
volatile int g_kb_caps_lock_down;
volatile int g_kb_caps_lock_toggled;

enum keyboard_ps2_state {
    KB_PS2_STATE_NORMAL,
    KB_PS2_STATE_E0_READ,
};

enum keyboard_ps2_state g_kb_ps2_state;

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

void update_modifier_leds() {
    uint8_t scroll_lock_set = (g_kb_modifiers & KB_MODIFIER_SCROLL_LOCK) == KB_MODIFIER_SCROLL_LOCK;
    uint8_t num_lock_set = (g_kb_modifiers & KB_MODIFIER_NUM_LOCK) == KB_MODIFIER_NUM_LOCK;
    uint8_t caps_lock_set = (g_kb_modifiers & KB_MODIFIER_CAPS_LOCK) == KB_MODIFIER_CAPS_LOCK;
    outb(PS2_KEYBOARD_DATA_PORT, 0xed);
    outb(PS2_KEYBOARD_DATA_PORT, (scroll_lock_set << 0) | (num_lock_set << 1) | (caps_lock_set << 2));
}

void toggle_caps_lock() {
    g_kb_modifiers ^= KB_MODIFIER_CAPS_LOCK;
    update_modifier_leds();
}

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
            if ((g_kb_modifiers & KB_MODIFIER_CAPS_LOCK) == 0) {
                toggle_caps_lock();
                g_kb_caps_lock_toggled = 1;
            }
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
        case KEY_CAPS_LOCK:
            if (g_kb_caps_lock_toggled)
                g_kb_caps_lock_toggled = 0;
            else
                toggle_caps_lock();
            break;
        default:
            break;
    }
    g_kb_on_key_release(key, keycode_to_char(g_kb_keymap, key, g_kb_modifiers));
}

void handle_keyboard() {
    uint8_t raw = inb(PS2_KEYBOARD_DATA_PORT);
    // multi byte scancode
    if (raw == 0xe0) {
        g_kb_ps2_state = KB_PS2_STATE_E0_READ;
        return;
    }
    // todo implement
    if (g_kb_ps2_state == KB_PS2_STATE_E0_READ) {
        g_kb_ps2_state = KB_PS2_STATE_NORMAL;
        return;
    }
    uint32_t keycode = ps2_scancodes_get(g_kb_keymap)[raw];
    if (keycode == 0)
        return;
    if ((keycode & KEY_RELEASE) == KEY_RELEASE)
        key_release(keycode & ~KEY_RELEASE);
    else
        key_press(keycode);
}

void kb_init() {
    g_kb_modifiers = KB_MODIFIER_NONE;
    g_kb_on_key_press = 0;
    g_kb_on_key_release = 0;
    g_kb_keymap = KEYMAP_US;
    g_kb_shift_left_down = 0;
    g_kb_shift_right_down = 0;
    g_kb_caps_lock_down = 0;
    g_kb_caps_lock_toggled = 0;
    g_kb_ps2_state = KB_PS2_STATE_NORMAL;

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
    update_modifier_leds();
    // enable scancodes
    outb(PS2_KEYBOARD_DATA_PORT, 0xf4);
    sti();
}
