#include <OctoWS2811.h>
#include "wheel.h"
#include <stdlib.h>

int rainbow_time;

int rgb(int r, int g, int b)
{
    return (r << 16) | (g << 8) | b;
}

void setup_rainbow() {

    rainbow_time = 0;
}

void loop_rainbow() {

    for (int j=0; j<WHEEL_LEN; j++) {
        //int color = rainbow_colors[(i+rainbow_time) % WHEEL_LEN];
        int color;
        int i = (j+5*rainbow_time) % WHEEL_LEN;
        if (i < 128) {
            color = rgb((255 - ((255*i)/127)), 255, 255);
        } else if (i<256) {
            color = rgb(0, (255 - 255*(i - 128)/127), 255);
        } else if (i < 384) {
            color = rgb(255*(i-256)/127, 0, 255);
        } else if (i < 512) {
            color = rgb(255, 0, (255 - 255*(i-384)/127));
        } else if (i < 640) {
            color = rgb(255, 255*(i-512)/127, 0);
        } else if (i < 768) {
            color = rgb(255-255*(i-640)/127, 255, 0);
        } else if (i < 896) {
            color = rgb(255*(i-768)/127, 255, 255*(i-768)/127);
        }
        leds.setPixel(wheel_idx_to_led_num(j, true), color);
        leds.setPixel(wheel_idx_to_led_num(j, false), color);
    }
    rainbow_time++;
    leds.show();

    delayMicroseconds(25 * 1000);
}
