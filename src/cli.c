#include "cli.h"
#include "terminal.h"
#include "rand.h"
#include "pit.h"

void enter_cli() {
    int n;
    while (1) {
        t_write("Anna arvattava kokonaisluku väliltä 1-100: ");
        int err = 0;
        n = str_to_int_dec(t_scan_line(), &err);
        if (!err && n >= 1 && n <= 100)
            break;
    }
    int guess = 0;
    int i = 0;
    srand(pit_get_ticks());
    while (guess != n) {
        guess = rand() % 100 + 1;
        i++;
    }
    t_write("Arvauksia tarvittiin ");
    t_write_dec(i);
    t_write(" kpl.\n");
}
