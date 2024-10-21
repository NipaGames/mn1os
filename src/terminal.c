#include "terminal.h"
#include "io.h"

uint16_t* g_t_buf;
volatile size_t g_t_pos_x;
volatile size_t g_t_pos_y;
volatile size_t g_t_tabw;
volatile uint8_t g_t_color;
char g_t_input_line[256];
volatile size_t g_t_input_pos;
volatile size_t g_t_input_start_tabw;
volatile int g_t_scanning;
volatile int g_t_waiting_for_input;

void t_erase(size_t pos, size_t count) {
    while (count > 0) {
        if (pos >= VGA_HEIGHT * VGA_WIDTH)
            break;
        g_t_buf[pos++] = vga_entry(' ', g_t_color);
        count--;
    }
}

void t_clear() {
    t_erase(0, VGA_WIDTH * VGA_HEIGHT);
    g_t_pos_x = 0;
    g_t_pos_y = 0;
    g_t_tabw = TERMINAL_TAB_WIDTH;
}

void t_scroll(size_t rows) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            size_t y_from = y + rows;
            g_t_buf[y * VGA_WIDTH + x] = 
                (y_from < VGA_HEIGHT) ? g_t_buf[y_from * VGA_WIDTH + x]: vga_entry(' ', g_t_color);
        }
    }
    g_t_pos_y -= rows;
}

size_t t_search_next_free_ln() {
    size_t ln = 0;
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            uint16_t* pos = &g_t_buf[y * VGA_WIDTH + x];
            uint8_t c;
            uint8_t color;
            vga_get(*pos, &c, &color);
            if (c != ' ') {
                ln = y + 1;
                break;
            }
        }
    }
    return ln;
}

void t_init() {
    g_t_buf = (uint16_t*) VGA_BUFFER_ADDR;
    g_t_pos_x = 0;
    g_t_pos_y = 0;
    g_t_tabw = 0;
    g_t_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    g_t_input_pos = 0;
    g_t_scanning = 0;
    t_sync_cursor_pos();
}

void t_hook_keyboard() {
    kb_on_key_press(t_key_press);
    kb_on_key_release(t_key_release);
}

void t_set_pos(size_t x, size_t y) {
    g_t_pos_x = x;
    g_t_pos_y = y;
}

void t_get_pos(size_t* x, size_t* y) {
    *x = g_t_pos_x;
    *y = g_t_pos_y;
}

void t_set_color(uint8_t color) {
    g_t_color = color;
}

void t_get_color(uint8_t* color) {
    *color = g_t_color;
}

void t_newline() {
    g_t_pos_x = 0;
    if (++g_t_pos_y == VGA_HEIGHT)
        t_scroll(1);
    g_t_tabw = TERMINAL_TAB_WIDTH;
}

void t_tab() {
    size_t tabw = g_t_tabw;
    for (int j = 0; j < tabw; j++)
        t_put_char(' ');
    g_t_tabw = TERMINAL_TAB_WIDTH;
}

void t_put_char(char c) {
    if (c == '\t') {
        t_tab();
        return;
    }
    else if (c == '\n') {
        t_newline();
        return;
    }

    if (g_t_pos_x == VGA_WIDTH) {
        g_t_pos_x = 0;
        if (++g_t_pos_y == VGA_HEIGHT)
            t_scroll(1);
    }
    const size_t index = g_t_pos_y * VGA_WIDTH + g_t_pos_x;
    g_t_buf[index] = vga_entry(c, g_t_color);
    g_t_pos_x++;
    if (--g_t_tabw == 0)
        g_t_tabw = TERMINAL_TAB_WIDTH;
}

int is_utf8(WCHAR c) {
    uint8_t byte1 = c >> 8 & 0xff;
    uint8_t byte2 = c & 0xff;
    return (byte1 & 0xc0) == 0xc0 && (byte2 & 0xc0) == 0x80;
}

uint8_t utf8_to_cp437(WCHAR c) {
    switch (c) {
        case 0xc2a1: return 0xad; // ¡
        case 0xc2a2: return 0x9b; // ¢
        case 0xc2a3: return 0x9c; // £
        case 0xc2a5: return 0x9d; // ¥
        case 0xc2a7: return 0x15; // §
        case 0xc2aa: return 0xa6; // ª
        case 0xc2ab: return 0xae; // «
        case 0xc2ac: return 0xaa; // ¬
        case 0xc2b0: return 0xf8; // °
        case 0xc2b1: return 0xf1; // ±
        case 0xc2b2: return 0xfd; // ²
        case 0xc2b5: return 0xe6; // µ
        case 0xc2b6: return 0x14; // ¶
        case 0xc2b7: return 0xfa; // ·
        case 0xc2ba: return 0xa7; // º
        case 0xc2bb: return 0xaf; // »
        case 0xc2bc: return 0xac; // ¼
        case 0xc2bd: return 0xab; // ½
        case 0xc2bf: return 0xa8; // ¿
        case 0xc384: return 0x8e; // Ä
        case 0xc385: return 0x8f; // Å
        case 0xc386: return 0x92; // Æ
        case 0xc387: return 0x80; // Ç
        case 0xc389: return 0x90; // É
        case 0xc391: return 0xa5; // Ñ
        case 0xc396: return 0x99; // Ö
        case 0xc39c: return 0x9a; // Ü
        case 0xc39f: return 0xe1; // ß
        case 0xc3a0: return 0x85; // à
        case 0xc3a1: return 0xa0; // á
        case 0xc3a2: return 0x83; // â
        case 0xc3a4: return 0x84; // ä
        case 0xc3a5: return 0x86; // å
        case 0xc3a6: return 0x91; // æ
        case 0xc3a7: return 0x87; // ç
        case 0xc3a8: return 0x8a; // è
        case 0xc3a9: return 0x82; // é
        case 0xc3aa: return 0x88; // ê
        case 0xc3ab: return 0x89; // ë
        case 0xc3ac: return 0x8d; // ì
        case 0xc3ad: return 0xa1; // í
        case 0xc3ae: return 0x8c; // î
        case 0xc3af: return 0x8b; // ï
        case 0xc3b1: return 0xa4; // ñ
        case 0xc3b2: return 0x95; // ò
        case 0xc3b3: return 0xa2; // ó
        case 0xc3b4: return 0x93; // ô
        case 0xc3b6: return 0x94; // ö
        case 0xc3b7: return 0xf6; // ÷
        case 0xc3b9: return 0x97; // ù
        case 0xc3ba: return 0xa3; // ú
        case 0xc3bb: return 0x96; // û
        case 0xc3bc: return 0x81; // ü
        case 0xc3bf: return 0x98; // ÿ
        default: return 0xfe; // not in cp437, show an awkward box lol
    }
}

void t_put_char_utf8(WCHAR c) {
    t_put_char(is_utf8(c) ? utf8_to_cp437(c) : (uint8_t) c >> 8 & 0xff);
}

const char g_t_digits[16] = "0123456789abcdef";
void t_put_digit(int digit) {
    if (digit < 0 || digit > 15) {
        T_WRITE_DIAGNOSTIC_STUB();
        t_write("invalid digit! (0-15 allowed, ");
        t_write_dec(digit);
        t_write(" given)");
        return;
    }

    t_put_char(g_t_digits[digit]);
}

void t_write_s(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        uint8_t c = data[i];
        if ((c & 0xc0) == 0xc0 && i != size - 1)
            t_put_char_utf8((uint16_t) c << 8 | (uint8_t) data[++i]);
        else
            t_put_char(c);
    }
}

void t_write(const char* data) {
    t_write_s(data, strlen(data));
}

void t_write_num(int num, int base) {
    if (base <= 0 || base > 16) {
        T_WRITE_DIAGNOSTIC_STUB();
        t_write("invalid base! (1-16 allowed, ");
        t_write_dec(base);
        t_write(" given)\n");
        return;
    }

    if (num < 0) {
        t_put_char('-');
        num = -num;
    }
    unsigned int next = num / (unsigned int) base;
    if (next != 0)
        t_write_num(next, base);
    t_put_digit(num % (unsigned int) base);
}

void t_write_diagnostic_stub(const char* file, int line) {
    t_write(file);
    t_put_char(':');
    t_write_dec(line);
}

void t_set_cursor_pos(size_t x, size_t y) {
    uint16_t pos = y * VGA_WIDTH + x;
    outb(0x3d4, 0x0f);
    outb(0x3d5, (uint8_t) (pos & 0xff));
    outb(0x3d4, 0x0e);
    outb(0x3d5, (uint8_t) ((pos >> 8) & 0xff));
}

void t_sync_cursor_pos() {
    t_set_cursor_pos(g_t_pos_x, g_t_pos_y);
}

void t_set_cursor(enum text_mode_cursor cursor) {
    outb(0x3d4, 0x0a);
    outb(0x3d5, (inb(0x3d5) & 0xc0) | ((cursor & 0xf0) >> 4));
    outb(0x3d4, 0x0b);
    outb(0x3d5, (inb(0x3d5) & 0xe0) | (cursor & 0x0f));
}

void t_hide_cursor() {
    outb(0x3d4, 0x0a);
    outb(0x3d5, 0x20);
}

void t_key_press(uint32_t key) {
    if (g_t_waiting_for_input) {
        g_t_waiting_for_input = 0;
        return;
    }
    if (!g_t_scanning)
        return;
    if (key == KEY_ENTER) {
        g_t_input_line[g_t_input_pos] = '\0';
        g_t_scanning = 0;
        return;
    }
    else if (key == KEY_BACKSPACE) {
        if (g_t_input_pos == 0)
            return;
        char prev;
        do {
            prev = g_t_input_line[--g_t_input_pos];
            g_t_input_line[g_t_input_pos] = '\0';
        } while ((prev & 0xc0) == 0x80);
        size_t pos = g_t_pos_y * VGA_WIDTH + g_t_pos_x;
        size_t len = 1;
        // please don't. just don't.
        if (prev == '\t') {
            size_t n = TERMINAL_TAB_WIDTH - g_t_input_start_tabw;
            size_t i = g_t_input_pos;
            while (i != 0) {
                uint8_t c = g_t_input_line[--i];
                if (c == '\t') {
                    n += g_t_input_start_tabw;
                    break;
                }
                // utf-8 second byte
                if ((c & 0xc0) != 0x80)
                    n++;
            }
            len = TERMINAL_TAB_WIDTH - (n % TERMINAL_TAB_WIDTH);
            g_t_tabw = len;
        }
        else if (g_t_tabw++ == TERMINAL_TAB_WIDTH)
            g_t_tabw = 1;
        size_t start_pos = pos - len;
        t_erase(start_pos, len);
        g_t_pos_y = start_pos / VGA_WIDTH;
        g_t_pos_x = start_pos % VGA_WIDTH;
    }
    else {
        if (g_t_input_pos == sizeof(g_t_input_line) - 1)
            return;
        WCHAR c = keycode_to_char(key);
        if (c == '\0')
            return;
        uint8_t byte1 = (c & 0xff00) >> 8;
        uint8_t byte2 = (c & 0x00ff);
        if (is_utf8(c)) {
            if (g_t_input_pos != sizeof(g_t_input_line) - 2) {
                g_t_input_line[g_t_input_pos++] = byte1;
                g_t_input_line[g_t_input_pos++] = byte2;
                t_put_char_utf8(c);
            }
        }
        else {
            g_t_input_line[g_t_input_pos++] = byte1;
            t_put_char(byte1);
        }
    }
    t_sync_cursor_pos();
}

void t_key_release(uint32_t key) {
}

const char* t_scan_line() {
    t_sync_cursor_pos();
    t_set_cursor(TEXT_MODE_CURSOR_UNDERLINE);
    g_t_input_pos = 0;
    g_t_input_start_tabw = g_t_tabw;
    g_t_scanning = 1;
    while (g_t_scanning);
    t_hide_cursor();
    t_newline();
    return g_t_input_line;
}

void t_wait_for_input() {
    t_sync_cursor_pos();
    t_set_cursor(TEXT_MODE_CURSOR_UNDERLINE);
    g_t_waiting_for_input = 1;
    while (g_t_waiting_for_input);
    t_hide_cursor();
}
