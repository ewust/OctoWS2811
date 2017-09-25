
#include "wheel.h"

// Map wheel idx to led num 

int wheel_idx_to_led_num(int idx, bool is_top)
{
    int num = 0;
    if (!is_top) {
        // a->f, b->g, c->h, etc
        num += 5*SECTION_LEN;
    }
    if (idx >= 3*SECTION_LEN) {
        // Second "half" of wheel
        // a->d, b->e, c->?
        // f->i, g->j, h->?

        // i.e.:
        //num += 3*SECTION_LEN;
        //num += idx - 3*SECTION_LEN
        // which is just
        num += idx;
    } else {
        // reverse
        if (idx < SECTION_LEN) {
            // Section 1
            idx += 2*SECTION_LEN;
        } else if (idx < 2*SECTION_LEN) {
            // Section 2
        } else if (idx < 3*SECTION_LEN) {
            // Section 3
            idx -= 2*SECTION_LEN;
        }
        num += (((3*SECTION_LEN) - 1) - idx);
    }
    return num;
}


/*
int wheel_idx_to_led_num(int idx, bool is_top)
{
    // sanity check: idx < SECTION_LEN*5?

    int num = 0;
    if (!is_top) {
        // a -> c, b -> d
        num += STRIP_LEN*2;
    }
    if (idx > STRIP_LEN) {
        // Second "half" of wheel
        // a -> b
        // c -> d

        // e.g.:
        //num += STRIP_LEN;
        //num += idx - STRIP_LEN
        // which is just
        num += idx;
    } else {
        // reverse
        num += ((STRIP_LEN - 1) - idx);
    }
    return num;
}
*/
