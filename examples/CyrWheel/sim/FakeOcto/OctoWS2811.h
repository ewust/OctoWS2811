#ifndef FAKEOCTO_H
#define FAKEOCTO_H

#include "wheel.h"
#include <cstddef>
#include <stdio.h>
#include <string.h>



class OctoWS2811 {
public:
    OctoWS2811(int strip_len, void *dispMem, void *drawMem, int config);
    ~OctoWS2811();

    void begin();
    void show();
    void setPixel(int idx, int color);
    int getPixel(int idx);

private:
    int *wheel_leds;
    int wheel_len;
};

extern OctoWS2811 leds;
extern int rainbowColors[180];

// defined in sim.c
void delayMicroseconds(size_t us);

#endif
