#include "pit.h"
#include "irq.h"

uint32_t g_pit_ticks = 0;

void handle_pit_tick() {
    g_pit_ticks++;
}

uint32_t pit_get_ticks() {
    return g_pit_ticks;
}

void pit_init() {
    irq_assign_handler(0, handle_pit_tick);
}
