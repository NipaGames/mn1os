#include "pic.h"
#include "io.h"

enum PIC_ICW1 {
    PIC_ICW1_ICW4           = 0x01,
    PIC_ICW1_SINGLE         = 0x02,
    PIC_ICW1_INTERVAL4      = 0x04,
    PIC_ICW1_LEVEL          = 0x08,
    PIC_ICW1_INIT           = 0x10
};

enum PIC_ICW4 {
    PIC_ICW4_8086           = 0x1,
    PIC_ICW4_AUTO_EOI       = 0x2,
    PIC_ICW4_BUFFER_MASTER  = 0x4,
    PIC_ICW4_BUFFER_SLAVE   = 0x0,
    PIC_ICW4_BUFFERED       = 0x8,
    PIC_ICW4_SFNM           = 0x10
};

enum PIC_CMD {
    PIC_CMD_END_INTERRUPT   = 0x20,
    PIC_CMD_READ_IRR        = 0x0a,
    PIC_CMD_READ_ISR        = 0x0b
};

void pic_init(uint8_t pic1, uint8_t pic2) {
    // icw1
    outb(PIC1_CMD_PORT, PIC_ICW1_ICW4 | PIC_ICW1_INIT);
    iowait();
    outb(PIC2_CMD_PORT, PIC_ICW1_ICW4 | PIC_ICW1_INIT);
    iowait();

    // icw2
    outb(PIC1_DATA_PORT, pic1);
    iowait();
    outb(PIC2_DATA_PORT, pic2);
    iowait();

    // icw3
    outb(PIC1_DATA_PORT, 0x4);
    iowait();
    outb(PIC2_DATA_PORT, 0x2);
    iowait();

    // icw4
    outb(PIC1_DATA_PORT, PIC_ICW4_8086);
    iowait();
    outb(PIC2_DATA_PORT, PIC_ICW4_8086);
    iowait();

    outb(PIC1_DATA_PORT, 0);
    iowait();
    outb(PIC2_DATA_PORT, 0);
    iowait();
}

void pic_end_interrupt(int irq) {
    if (irq >= 8)
        outb(PIC2_CMD_PORT, PIC_CMD_END_INTERRUPT);
    outb(PIC1_CMD_PORT, PIC_CMD_END_INTERRUPT);
}

