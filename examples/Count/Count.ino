#include <OctoWS2811.h>


#define SECTION_LEN  177
#define IDX 150

DMAMEM uint16_t displayMemory[SECTION_LEN*24];
uint16_t drawingMemory[SECTION_LEN*24];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(SECTION_LEN, displayMemory, drawingMemory, config);

int fade;
int dir;


int wheel_idx_to_led_num(int idx, bool is_top)
{
    int num = 0;
    if (!is_top) {
        // a->f, b->g, c->h, etc
        num += 5*SECTION_LEN;
    }
    if (idx > 3*SECTION_LEN) {
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
        // 0 - 177 should be reversed (177 - idx)
        // 178 - (178+177) should be (177+178)-idx
        if (idx < SECTION_LEN) {
          num += (SECTION_LEN - idx);
        } else if (idx < 2*SECTION_LEN) {
          num += (3*SECTION_LEN - idx);
        } else {
          num += (4*SECTION_LEN - idx);
        }
    }
    return num;
}


int s_idx = 0;
void setup() {
    leds.begin();
    fade = 0;
    dir = 1;
    s_idx = IDX;
}

void loop() {

    fade += dir;

    if (fade >= 0x80) {
        fade = 0x80;
        dir = -1;
        s_idx++;
    } else if (fade <= 0) {
        fade = 0;
        dir = 1;
    }

    int idx = wheel_idx_to_led_num(s_idx, true);

    idx = 177;

  
    leds.setPixel(idx-1, fade << 16);
    leds.setPixel(idx, fade << 8);
    leds.setPixel(idx+1, fade);
    leds.show();

    
    delayMicroseconds(3*1000);


    
}
