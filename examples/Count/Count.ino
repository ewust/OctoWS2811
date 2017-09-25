#include <OctoWS2811.h>
#include "wheel.h"

#define IDX 150

DMAMEM int displayMemory[STRIP_LEN * 12];
int drawingMemory[STRIP_LEN * 12];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(STRIP_LEN, displayMemory, drawingMemory, config);

int fade;
int dir;

int s_idx = 0;
void setup() {

  
    while (!Serial && (millis() < 2000)) ;
    Serial.begin(38400);
  
    Serial.println("Setup Called...");
    Serial.flush();
    
    leds.begin();
    fade = 0;
    dir = 1;
    s_idx = 0;
}

void set_both(int idx, int fade)
{
    int n = wheel_idx_to_led_num(idx, true);
    leds.setPixel(n - 1, fade << 16);
    leds.setPixel(n, fade << 8);
    leds.setPixel(n+1, fade);

    n = wheel_idx_to_led_num(idx, false);
    leds.setPixel(n - 1, fade << 12);
    leds.setPixel(n, fade << 4);
    leds.setPixel(n+1, fade << 2);
}

void loop() {

    fade += dir;

    if (fade >= 0x80) {
        fade = 0x80;
        dir = -1;
        //s_idx++;
    } else if (fade <= 0) {
        fade = 0;
        dir = 1;
    }
    s_idx++;
    if (s_idx >= WHEEL_LEN) s_idx = 0;
    //set_both(s_idx, fade);


    int color = 0x40;
    if (s_idx < STRIP_LEN) color = 0x000040;        // Blue (Section 1) (176)
    else if (s_idx < 2*STRIP_LEN) color = 0x004000; // Green (Section 2)
    else if (s_idx < 3*STRIP_LEN) color = 0x400000; // Red (Section 3)
    else if (s_idx < 4*STRIP_LEN) color = 0x004040; // Teal (Section 4)
    else if (s_idx < 5*STRIP_LEN) color = 0x400040; // Purple (Seciton 5)
    int i;
    for (i=0; i<WHEEL_LEN; i++) {
      //leds.setPixel(wheel_idx_to_led_num(i, true), 0x00);
    }
    leds.setPixel(wheel_idx_to_led_num(s_idx, false), color);
    leds.setPixel(wheel_idx_to_led_num(s_idx, true), color);
    /*
    // Section 3
    set_both(0, fade);  // Left side
    set_both(175, fade);

    // Section 4
    //set_both(176, fade);
    //set_both(176+175, fade);

    // Section 3
    set_both(177*3, fade);

    // Section 4
    //set_both(177*4, fade);

    // Section 5
    //set_both(177*5, fade);
    */
    leds.show();

    
    delayMicroseconds(10*1000);


    
}
