#include "keyboard.h"
#include "io.h"
#include "irq.h"
#include "keymaps.h"

#define PS2_KEYBOARD_DATA_PORT 0x60
#define PS2_KEYBOARD_CMD_PORT 0x64

enum keyboard_modifiers g_kb_modifiers;
kb_key_event_handler g_kb_on_key_press;
kb_key_event_handler g_kb_on_key_release;
enum keymap g_kb_keymap;

volatile int g_kb_shift_left_down;
volatile int g_kb_shift_right_down;
volatile int g_kb_ctrl_left_down;
volatile int g_kb_ctrl_right_down;
volatile int g_kb_alt_gr_down;
volatile int g_kb_scroll_lock_down;
volatile int g_kb_scroll_lock_toggled;
volatile int g_kb_num_lock_down;
volatile int g_kb_num_lock_toggled;
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
void kb_set_keymap(uint8_t keys) {
    g_kb_keymap = keys;
}
uint8_t kb_get_keymap() {
    return g_kb_keymap;
}

void update_modifier_leds() {
    outb(PS2_KEYBOARD_DATA_PORT, 0xed);
    outb(PS2_KEYBOARD_DATA_PORT, g_kb_modifiers & 0b111);
}

int is_modifier_set(enum keyboard_modifiers modifier) {
    return (g_kb_modifiers & modifier) == modifier;
}

void toggle_modifier(enum keyboard_modifiers modifier) {
    g_kb_modifiers ^= modifier;
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
        case KEY_CTRL_LEFT:
            if (g_kb_ctrl_left_down)
                return;
            g_kb_ctrl_left_down = 1;
            break;
        case KEY_CTRL_RIGHT:
            if (g_kb_ctrl_right_down)
                return;
            g_kb_ctrl_right_down = 1;
            break;
        case KEY_ALT_GR:
            if (g_kb_alt_gr_down)
                return;
            g_kb_alt_gr_down = 1;
            break;
        case KEY_SCROLL_LOCK:
            if (g_kb_scroll_lock_down)
                return;
            g_kb_scroll_lock_down = 1;
            break;
        case KEY_NUM_LOCK:
            if (g_kb_num_lock_down)
                return;
            g_kb_num_lock_down = 1;
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
        case KEY_CTRL_LEFT:
        case KEY_CTRL_RIGHT:
            g_kb_modifiers |= KB_MODIFIER_CTRL;
            break;
        case KEY_ALT_GR:
            g_kb_modifiers |= KB_MODIFIER_ALT_GR;
            break;
        case KEY_SCROLL_LOCK:
            if (!is_modifier_set(KB_MODIFIER_SCROLL_LOCK)) {
                toggle_modifier(KB_MODIFIER_SCROLL_LOCK);
                g_kb_scroll_lock_toggled = 1;
            }
            break;
        case KEY_NUM_LOCK:
            if (!is_modifier_set(KB_MODIFIER_NUM_LOCK)) {
                toggle_modifier(KB_MODIFIER_NUM_LOCK);
                g_kb_num_lock_toggled = 1;
            }
            break;
        case KEY_CAPS_LOCK:
            if (!is_modifier_set(KB_MODIFIER_CAPS_LOCK)) {
                toggle_modifier(KB_MODIFIER_CAPS_LOCK);
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
        case KEY_CTRL_LEFT:
            g_kb_ctrl_left_down = 0;
            break;
        case KEY_CTRL_RIGHT:
            g_kb_ctrl_right_down = 0;
            break;
        case KEY_ALT_GR:
            g_kb_alt_gr_down = 0;
            break;
        case KEY_SCROLL_LOCK:
            g_kb_scroll_lock_down = 0;
            break;
        case KEY_NUM_LOCK:
            g_kb_num_lock_down = 0;
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
        case KEY_CTRL_LEFT:
        case KEY_CTRL_RIGHT:
            if (!g_kb_ctrl_left_down && !g_kb_ctrl_right_down)
                g_kb_modifiers &= ~KB_MODIFIER_CTRL;
            break;
        case KEY_ALT_GR:
            g_kb_modifiers &= ~KB_MODIFIER_ALT_GR;
            break;
        case KEY_SCROLL_LOCK:
            if (g_kb_scroll_lock_toggled)
                g_kb_scroll_lock_toggled = 0;
            else
                toggle_modifier(KB_MODIFIER_SCROLL_LOCK);
            break;
        case KEY_NUM_LOCK:
            if (g_kb_num_lock_toggled)
                g_kb_num_lock_toggled = 0;
            else
                toggle_modifier(KB_MODIFIER_NUM_LOCK);
            break;
        case KEY_CAPS_LOCK:
            if (g_kb_caps_lock_toggled)
                g_kb_caps_lock_toggled = 0;
            else
                toggle_modifier(KB_MODIFIER_CAPS_LOCK);
            break;
        default:
            break;
    }
    g_kb_on_key_release(key, keycode_to_char(g_kb_keymap, key, g_kb_modifiers));
}

uint32_t get_e0_keycode(uint8_t raw) {
    switch (raw) {
        case 0x1c: return KEY_KEYPAD_ENTER | KEY_PRESS;
        case 0x1d: return KEY_CTRL_RIGHT | KEY_PRESS;
        case 0x35: return KEY_KEYPAD_SLASH | KEY_PRESS;
        case 0x38: return KEY_ALT_GR | KEY_PRESS;
        case 0x47: return KEY_HOME | KEY_PRESS;
        case 0x48: return KEY_UP | KEY_PRESS;
        case 0x49: return KEY_PAGE_UP | KEY_PRESS;
        case 0x4b: return KEY_LEFT | KEY_PRESS;
        case 0x4d: return KEY_RIGHT | KEY_PRESS;
        case 0x4f: return KEY_END | KEY_PRESS;
        case 0x50: return KEY_DOWN | KEY_PRESS;
        case 0x51: return KEY_PAGE_DOWN | KEY_PRESS;
        case 0x52: return KEY_INSERT | KEY_PRESS;
        case 0x53: return KEY_DELETE | KEY_PRESS;
        case 0x9c: return KEY_KEYPAD_ENTER | KEY_RELEASE;
        case 0x9d: return KEY_CTRL_RIGHT | KEY_RELEASE;
        case 0xb5: return KEY_KEYPAD_SLASH | KEY_RELEASE;
        case 0xb8: return KEY_ALT_GR | KEY_RELEASE;
        case 0xc7: return KEY_HOME | KEY_RELEASE;
        case 0xc8: return KEY_UP | KEY_RELEASE;
        case 0xc9: return KEY_PAGE_UP | KEY_RELEASE;
        case 0xcb: return KEY_LEFT | KEY_RELEASE;
        case 0xcd: return KEY_RIGHT | KEY_RELEASE;
        case 0xcf: return KEY_END | KEY_RELEASE;
        case 0xd0: return KEY_DOWN | KEY_RELEASE;
        case 0xd1: return KEY_PAGE_DOWN | KEY_RELEASE;
        case 0xd2: return KEY_INSERT | KEY_RELEASE;
        case 0xd3: return KEY_DELETE | KEY_RELEASE;
        default: return KEY_UNKNOWN;
    }
}

void handle_keyboard() {
    uint8_t raw = inb(PS2_KEYBOARD_DATA_PORT);
    // multi byte scancode
    uint32_t keycode;
    // todo implement
    if (g_kb_ps2_state == KB_PS2_STATE_E0_READ) {
        g_kb_ps2_state = KB_PS2_STATE_NORMAL;
        keycode = get_e0_keycode(raw);
    }
    else {
        if (raw == 0xe0) {
            g_kb_ps2_state = KB_PS2_STATE_E0_READ;
            return;
        }
        keycode = ps2_scancodes_get(g_kb_keymap)[raw];
    }
    if (keycode == KEY_UNKNOWN)
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
    g_kb_ctrl_left_down = 0;
    g_kb_ctrl_right_down = 0;
    g_kb_alt_gr_down = 0;
    g_kb_scroll_lock_down = 0;
    g_kb_scroll_lock_toggled = 0;
    g_kb_num_lock_down = 0;
    g_kb_num_lock_toggled = 0;
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

enum keyboard_modifiers kb_get_modifiers() {
    return g_kb_modifiers;
}
