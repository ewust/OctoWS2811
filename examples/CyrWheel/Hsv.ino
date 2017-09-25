#include <OctoWS2811.h>
#include "wheel.h"
#include <stdlib.h>

int hsv_time;

void setup_hsv() {
    hsv_time = 0;
}

void loop_hsv() {

    for (int i=0; i<WHEEL_LEN; i++) {
        int idx = (i+4*hsv_time) % WHEEL_LEN;
        int color = rainbowColors[idx*180/WHEEL_LEN];
        leds.setPixel(wheel_idx_to_led_num(i, true), color);
        leds.setPixel(wheel_idx_to_led_num(i, false), color);
    }
    hsv_time++;
    leds.show();

    delayMicroseconds(25 * 1000);
}
