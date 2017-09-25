#include <OctoWS2811.h>
#include "wheel.h"

int temps[WHEEL_LEN];

// TODO: move to a generic util function
// Colors are (red << 16) | (green << 8) | blue
int blackBodyColor(float temp)
{
    float x = temp / 1000;
    float x2 = x*x;
    float x3 = x2*x;
    float x4 = x3*x;
    float x5 = x4*x;

    float a = x3;

    float R = 0, G = 0, B = 0;

    // red
    if (temp <= 6600) {
        R = 1;
    } else {
        R = 0.0002889 * x5 - 0.01258 * x4 + 0.2148 * x3 - 1.776 * x2 + 6.907 * x - 8.723;
    }

    // green
    if (temp <= 6600) {
        G = -0.00004593 * x5 + 0.001424 * x4 - 0.01489 * x3 + 0.0498 * x2 + 0.1669 * x - 0.1653;
    } else {
        G = -0.0000001308 * x5 + 0.00001745 * x4 - 0.0009116 * x3 + 0.02348 * x2 - 0.3048 * x + 2.159;
    }

    // blue
    if (temp <= 2000) {
        B = 0;
    } else if (temp < 6600) {
        B = 0.00001764 * x5 + 0.0003575 * x4 - 0.01554 * x3 + 0.1549 * x2 - 0.3682 * x + 0.2386;
    } else {
        B = 1;
    }

    if (a > 1) {
        a = 1;
    }

    if (R<0) R=0;
    if (G<0) G=0;
    if (B<0) B=0;

    a /= 1;

    int r = ((int)(a*R*255)) & 0xff;
    int g = ((int)(a*G*255)) & 0xff;
    int b = ((int)(a*B*255)) & 0xff;
    return (r << 16) | (g << 8) | b;
}

int blazers_pos;
int blazers_v;

void setup_blazers() {

    for (int i=0; i<WHEEL_LEN; i++) {
        temps[i] = 1000.0;
    }
    blazers_pos = WHEEL_LEN;
    blazers_v = 5;
}


void loop_blazers() {

    int i;
    for (i=0; i<blazers_v; i++) {
        // Update blazers_position
        blazers_pos++;
        if (blazers_pos >= 0 && blazers_pos < WHEEL_LEN) {
            temps[blazers_pos] += 7500;
        }
        if (blazers_pos > WHEEL_LEN) {
            blazers_pos = 0;
            blazers_v+=3;
            if (blazers_v > 40) blazers_v = 5;
        }
    }

    // Write colors
    for (i=0; i < WHEEL_LEN; i++) {
        float t = temps[i];
        int color = blackBodyColor(t);
        leds.setPixel(wheel_idx_to_led_num(i, true), color);
        leds.setPixel(wheel_idx_to_led_num(i, false), color);
        //leds.setPixel(i+j*5*144, color);
        temps[i]*=0.85;
    }
    leds.show();
    delayMicroseconds(30*1000);
}

