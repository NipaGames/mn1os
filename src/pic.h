#ifndef PIC_H
#define PIC_H

#include "types.h"

void pic_init(uint8_t pic1, uint8_t pic2);
void pic_end_interrupt(int irq);

#endif
