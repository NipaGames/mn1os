#include "terminal.h"
#include "io.h"
#include "utf8.h"

uint16_t* g_t_buf;
volatile size_t g_t_pos_x;
volatile size_t g_t_pos_y;
volatile size_t g_t_tabw;
volatile uint8_t g_t_color;
char g_t_input_line[256];
volatile size_t g_t_input_start_pos_x;
volatile size_t g_t_input_start_pos_y;
volatile size_t g_t_input_end_pos_x;
volatile size_t g_t_input_end_pos_y;
volatile size_t g_t_input_pos;
volatile size_t g_t_input_len;
volatile size_t g_t_input_start_tabw;
volatile int g_t_scanning;
volatile int g_t_waiting_for_input;

void t_set(size_t pos, uint16_t entry) {
    if (pos >= VGA_HEIGHT * VGA_WIDTH)
        return;
    g_t_buf[pos] = entry;
}

uint16_t t_get(size_t pos) {
    return g_t_buf[pos];
}

void t_fill(size_t pos, size_t count, uint16_t entry) {
    while (count > 0) {
        t_set(pos++, entry);
        count--;
    }
}

void t_erase(size_t pos, size_t count) {
    t_fill(pos, count, vga_entry(' ', g_t_color));
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
                (y_from < VGA_HEIGHT) ? g_t_buf[y_from * VGA_WIDTH + x] : vga_entry(' ', g_t_color);
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
    g_t_input_start_pos_x = 0;
    g_t_input_start_pos_y = 0;
    g_t_input_end_pos_x = 0;
    g_t_input_end_pos_y = 0;
    g_t_input_len = 0;
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

size_t get_input_prev_tabw() {
    size_t n = TERMINAL_TAB_WIDTH - g_t_input_start_tabw;
    size_t i = g_t_input_pos;
    // please don't. just don't.
    while (i != 0) {
        char c = g_t_input_line[--i];
        if (c == '\t') {
            n += g_t_input_start_tabw;
            break;
        }
        // utf-8 second byte
        if ((c & 0xc0) != 0x80)
            n++;
    }
    return TERMINAL_TAB_WIDTH - (n % TERMINAL_TAB_WIDTH);
}

size_t get_input_vga_buffer_len() {
    size_t end_pos = g_t_input_end_pos_y * VGA_WIDTH + g_t_input_end_pos_x;
    size_t start_pos = g_t_input_start_pos_y * VGA_WIDTH + g_t_input_start_pos_x;
    return end_pos - start_pos;
}

size_t input_go_back_char(size_t pos, int is_tab) {
    size_t len = 1;
    if (is_tab) {
        len = get_input_prev_tabw();
        g_t_tabw = len;
    }
    else if (g_t_tabw++ == TERMINAL_TAB_WIDTH)
        g_t_tabw = 1;
    size_t prev_pos = pos - len;
    g_t_pos_y = prev_pos / VGA_WIDTH;
    g_t_pos_x = prev_pos % VGA_WIDTH;
    return len;
}

size_t input_go_forward_char(size_t pos, int is_tab) {
    size_t len = 1;
    if (is_tab) {
        len = get_input_prev_tabw();
        g_t_tabw = TERMINAL_TAB_WIDTH;
    }
    else if (--g_t_tabw == 0)
        g_t_tabw = TERMINAL_TAB_WIDTH;
    size_t next_pos = pos + len;
    g_t_pos_y = next_pos / VGA_WIDTH;
    g_t_pos_x = next_pos % VGA_WIDTH;
    return len;
}

void input_shift_input_line_vga(size_t pos, int offset) {
    size_t end_pos = g_t_input_end_pos_y * VGA_WIDTH + g_t_input_end_pos_x;
    size_t len = end_pos - pos;
    for (size_t i = 0; i < len; i++) {
        size_t from;
        if (offset > 0)
            from = end_pos - i - 1;
        else
            from = pos + i;

        if (from + offset >= VGA_HEIGHT * VGA_WIDTH) {
            t_scroll(1);
            g_t_input_start_pos_y--;
            pos -= VGA_WIDTH;
            end_pos -= VGA_WIDTH;
            i--;
            continue;
        }
        g_t_buf[from + offset] = g_t_buf[from];
    }
    if (offset > 0)
        t_erase(pos, offset);
    else
        t_erase(end_pos + offset, -offset);
}

void input_shift_input_line(size_t pos, int offset) {
    for (size_t i = 0; i < g_t_input_len - pos; i++) {
        size_t from;
        if (offset > 0)
            from = g_t_input_len - i - 1;
        else
            from = pos + i - offset;
        g_t_input_line[from + offset] = g_t_input_line[from];
    }
    if (offset > 0) {
        for (int i = 0; i < offset; i++)
            g_t_input_line[pos + i] = '\0';
    }
    else {
        for (int i = 0; i < -offset; i++)
            g_t_input_line[g_t_input_len - 1 - i - offset] = '\0';
    }
}

void t_key_press(uint32_t key, WCHAR c) {
    if (g_t_waiting_for_input) {
        g_t_waiting_for_input = 0;
        return;
    }
    if (!g_t_scanning)
        return;
    size_t vga_buffer_pos = g_t_pos_y * VGA_WIDTH + g_t_pos_x;
    int is_ctrl_down = (kb_get_modifiers() & KB_MODIFIER_CTRL) == KB_MODIFIER_CTRL;
    if (key == KEY_ENTER || key == KEY_KEYPAD_ENTER) {
        if (g_t_input_len > 0) {
            g_t_pos_x = g_t_input_end_pos_x;
            g_t_pos_y = g_t_input_end_pos_y;
        }
        g_t_input_pos = g_t_input_len;
        g_t_input_line[g_t_input_len] = '\0';
        g_t_scanning = 0;
        return;
    }
    else if (key == KEY_LEFT || (is_ctrl_down && key == KEY_B) || key == KEY_BACKSPACE) {
        if (g_t_input_pos == 0)
            return;
        char prev;
        size_t clen = 0;
        do {
            prev = g_t_input_line[--g_t_input_pos];
            clen++;
            if (key == KEY_BACKSPACE) {
                g_t_input_line[g_t_input_pos] = '\0';
                g_t_input_len--;
            }
        } while ((prev & 0xc0) == 0x80);
        size_t len = input_go_back_char(vga_buffer_pos, prev == '\t');
        if (key == KEY_BACKSPACE) {
            if (g_t_input_pos == g_t_input_len) {
                t_erase(vga_buffer_pos - len, len);
            }
            else {
                input_shift_input_line_vga(vga_buffer_pos, -len);
                input_shift_input_line(g_t_input_pos, -clen);
            }
            size_t end_pos = g_t_input_end_pos_y * VGA_WIDTH + g_t_input_end_pos_x - len;
            g_t_input_end_pos_y = end_pos / VGA_WIDTH;
            g_t_input_end_pos_x = end_pos % VGA_WIDTH;
        }
    }
    else if (key == KEY_RIGHT || (is_ctrl_down && key == KEY_F)) {
        if (g_t_input_pos == g_t_input_len)
            return;
        char prev = g_t_input_line[g_t_input_pos];
        if ((prev & 0xc0) == 0xc0)
            prev = g_t_input_line[++g_t_input_pos];
        input_go_forward_char(vga_buffer_pos, prev == '\t');
        g_t_input_pos++;
    }
    else if (is_ctrl_down && key == KEY_A) {
        if (g_t_input_len == 0)
            return;
        g_t_pos_x = g_t_input_start_pos_x;
        g_t_pos_y = g_t_input_start_pos_y;
        g_t_input_pos = 0;
    }
    else if (is_ctrl_down && key == KEY_E) {
        if (g_t_input_len == 0)
            return;
        g_t_pos_x = g_t_input_end_pos_x;
        g_t_pos_y = g_t_input_end_pos_y;
        g_t_input_pos = g_t_input_len;
    }
    else {
        if (g_t_input_len == sizeof(g_t_input_line) - 1)
            return;
        if (c == 0x0000)
            return;
        uint8_t byte1 = (c & 0xff00) >> 8;
        uint8_t byte2 = (c & 0x00ff);
        size_t step = byte1 == '\t' ? g_t_tabw : 1;
        if (is_utf8(c)) {
            if (g_t_input_len == sizeof(g_t_input_line) - 2)
                return;
            if (g_t_input_pos != g_t_input_len) {
                input_shift_input_line_vga(vga_buffer_pos, step);
                input_shift_input_line(g_t_input_pos, 2);
            }
            g_t_input_line[g_t_input_pos++] = byte1;
            g_t_input_line[g_t_input_pos++] = byte2;
            g_t_input_len += 2;
            t_put_char_utf8(c);
        }
        else {
            if (g_t_input_pos != g_t_input_len) {
                input_shift_input_line_vga(vga_buffer_pos, step);
                input_shift_input_line(g_t_input_pos, 1);
            }
            g_t_input_line[g_t_input_pos++] = byte1;
            g_t_input_len++;
            t_put_char(byte1);
        }
        size_t end_pos = (g_t_input_end_pos_y + g_t_pos_y) * VGA_WIDTH + g_t_input_end_pos_x + g_t_pos_x - vga_buffer_pos;
        g_t_input_end_pos_y = end_pos / VGA_WIDTH;
        g_t_input_end_pos_x = end_pos % VGA_WIDTH;
    }
    while (g_t_pos_y * VGA_WIDTH + g_t_pos_x >= VGA_WIDTH * VGA_HEIGHT) {
        t_scroll(1);
        g_t_input_end_pos_x = 0;
        g_t_input_end_pos_y--;
        g_t_input_start_pos_y--;
    }
    t_sync_cursor_pos();
}

void t_key_release(uint32_t key, WCHAR c) {
}

char* t_scan_line() {
    t_sync_cursor_pos();
    t_set_cursor(TEXT_MODE_CURSOR_UNDERLINE);
    g_t_input_pos = 0;
    g_t_input_len = 0;
    g_t_input_start_pos_x = g_t_pos_x;
    g_t_input_start_pos_y = g_t_pos_y;
    g_t_input_end_pos_x = g_t_input_start_pos_x;
    g_t_input_end_pos_y = g_t_input_start_pos_y;
    g_t_input_start_tabw = g_t_tabw;
    g_t_scanning = 1;
    while (g_t_scanning);
    t_hide_cursor();
    t_newline();
    return g_t_input_line;
}

void t_wait_for_input() {
    g_t_waiting_for_input = 1;
    while (g_t_waiting_for_input);
}
