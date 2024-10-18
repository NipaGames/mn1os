#include "terminal.h"
#include "io.h"

uint16_t* g_t_buf;
size_t g_t_pos_x = 0;
size_t g_t_pos_y = 0;
uint8_t g_t_color;

void t_clear() {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            g_t_buf[y * VGA_WIDTH + x] = vga_entry(' ', g_t_color);
        }
    }
    g_t_pos_x = 0;
    g_t_pos_y = 0;
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
            char c;
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
    g_t_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    t_sync_cursor_pos();
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
    g_t_pos_y++;
    if (g_t_pos_y == VGA_HEIGHT)
        t_scroll(1);
}

void t_tab() {
    int spaces = g_t_pos_x % TERMINAL_TAB_WIDTH;
    if (spaces == 0)
        spaces = TERMINAL_TAB_WIDTH;
    for (int j = 0; j < spaces; j++)
        t_put_char(' ');
}

void t_put_char(char c) {
    if (g_t_pos_x == VGA_WIDTH)
        t_newline();
    const size_t index = g_t_pos_y * VGA_WIDTH + g_t_pos_x;
    g_t_buf[index] = vga_entry(c, g_t_color);
    g_t_pos_x++;
}

const char g_t_digits[16] = "0123456789abcdef";
void t_put_digit(int digit) {
    if (digit < 0 || digit > 16) {
        T_WRITE_DIAGNOSTIC_STUB();
        t_write("invalid digit! (0-16 allowed, ");
        t_write(" given)");
        return;
    }

    t_put_char(g_t_digits[digit]);
}

void t_write_s(const char* data, size_t size) {
    for (size_t i = 0; i < size; i++) {
        char c = data[i];
        if (c == '\t') {
            t_tab();
            continue;
        }
        else if (c == '\n') {
            t_newline();
            continue;
        }
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
    int next = num / base;
    if (next != 0)
        t_write_num(next, base);
    t_put_digit(num % base);
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

