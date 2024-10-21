#ifndef IO_H
#define IO_H

#include "types.h"

enum io_port {
    PIC1_CMD_PORT       = 0x20,
    PIC1_DATA_PORT      = 0x21,
    PIC2_CMD_PORT       = 0xa0,
    PIC2_DATA_PORT      = 0xa1
};

static inline void outb(uint16_t port, uint8_t data) {
    __asm__ volatile ("outb %b0, %w1" : : "a" (data), "Nd" (port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    __asm__ volatile ("inb %w1, %b0" : "=a" (ret) : "Nd" (port));
    return ret;
}

static inline void iowait() {
    outb(0x80, 0);
}

static inline void sti() {
    __asm__ volatile ("sti");
}

static inline void cli() {
    __asm__ volatile ("cli");
}

static inline void halt() {
    __asm__ volatile ("hlt");
loop:
    goto loop;
}

static inline void reboot() {
    cli();
    uint8_t good = 0x02;
    while (good & 0x02)
        good = inb(0x64);
    outb(0x64, 0xfe);
    halt();
}

#endif

