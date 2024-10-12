#ifndef TERMINAL_H
#define TERMINAL_H

#include "types.h"
#include "string.h"

#define VGA_BUFFER_ADDR 0xb8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

enum vga_color {
    VGA_COLOR_BLACK = 0x0,
    VGA_COLOR_BLUE = 0x1,
    VGA_COLOR_GREEN = 0x2,
    VGA_COLOR_CYAN = 0x3,
    VGA_COLOR_RED = 0x4,
    VGA_COLOR_MAGENTA = 0x5,
    VGA_COLOR_BROWN = 0x6,
    VGA_COLOR_LIGHT_GREY = 0x7,
    VGA_COLOR_DARK_GREY = 0x8,
    VGA_COLOR_LIGHT_BLUE = 0x9,
    VGA_COLOR_LIGHT_GREEN = 0xa,
    VGA_COLOR_LIGHT_CYAN = 0xb,
    VGA_COLOR_LIGHT_RED = 0xc,
    VGA_COLOR_LIGHT_MAGENTA = 0xd,
    VGA_COLOR_LIGHT_BROWN = 0xe,
    VGA_COLOR_WHITE = 0xf,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {
    return fg | bg << 4;
}
static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
    return (uint16_t) uc | (uint16_t) color << 8;
}
static inline void vga_get(uint16_t v, char* uc, uint8_t* color) {
    *uc = (char) v;
    *color = (uint8_t) v >> 8;
}

void    t_clear();
void    t_scroll(size_t rows);
size_t  t_search_next_free_ln();
void    t_init();
void    t_set_pos(size_t x, size_t y);
void    t_get_pos(size_t* x, size_t* y);
void    t_set_color(uint8_t color);
void    t_get_color(uint8_t* color);
void    t_putc(char c);
void    t_newline();
void    t_write_s(const char* data, size_t size);
void    t_write(const char* data);

enum text_mode_cursor {
    TEXT_MODE_CURSOR_NONE = 0x00,
    TEXT_MODE_CURSOR_INSERT = 0x0f,
    TEXT_MODE_CURSOR_INPUT = 0xef,
};

void    t_set_cursor_pos(size_t x, size_t y);
void    t_sync_cursor_pos();
void    t_set_cursor(enum text_mode_cursor cursor);
static inline void t_hide_cursor() {
    t_set_cursor(TEXT_MODE_CURSOR_NONE);
}

#endif

