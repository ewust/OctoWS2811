#include "OctoWS2811.h"
#include <stdlib.h>


// Global, defined by the sim..
void display();

OctoWS2811::OctoWS2811(int strip_len, void *dispMem, void *drawMem, int config)
{
    wheel_len = strip_len;
    wheel_leds = (int*)malloc(sizeof(int)*strip_len);
}

OctoWS2811::~OctoWS2811()
{
    free(wheel_leds);
}

void OctoWS2811::begin() {
}

void OctoWS2811::show() {
    // Call OpenGL display
    display();
}

void OctoWS2811::setPixel(int idx, int color)
{
    wheel_leds[idx] = color;
}

int OctoWS2811::getPixel(int idx)
{
    return wheel_leds[idx];
}
