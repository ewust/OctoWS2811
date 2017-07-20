#include <OctoWS2811.h>


#define STRAND_LEN  244
#define IDX 177

DMAMEM int displayMemory[STRAND_LEN*6];
int drawingMemory[STRAND_LEN*6];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(STRAND_LEN, displayMemory, drawingMemory, config);

int fade;
int dir;

void setup() {
    leds.begin();
    fade = 0;
    dir = 1;
}

void loop() {

    fade += dir;

    if (fade >= 0x80) {
        fade = 0x80;
        dir = -1;
    } else if (fade <= 0) {
        fade = 0;
        dir = 1;
    }
  
    leds.setPixel(IDX-1, fade << 16);
    leds.setPixel(IDX, fade << 8);
    leds.setPixel(IDX+1, fade);
    leds.show();

    
    delayMicroseconds(3*1000);
}
