
/*
  Required Connections
  --------------------
    pin 2:  LED Strip #1    OctoWS2811 drives 8 LED Strips.
    pin 14: LED strip #2    All 8 are the same length.
    pin 7:  LED strip #3
    pin 8:  LED strip #4    A 100 ohm resistor should used
    pin 6:  LED strip #5    between each Teensy pin and the
    pin 20: LED strip #6    wire to the LED strip, to minimize
    pin 21: LED strip #7    high frequency ringining & noise.
    pin 5:  LED strip #8
    pin 15 & 16 - Connect together, but do not use
    pin 4 - Do not use
    pin 3 - Do not use as PWM.  Normal use is ok.
    pin 1 - Output indicating CPU usage, monitor with an oscilloscope,
            logic analyzer or even an LED (brighter = CPU busier)
*/

#include <OctoWS2811.h>





#define SECTION_LEN 178             // number of pixels in one section
#define STRIP_LEN (SECTION_LEN*3)   // longest strip length. Pretend all are equal (even though some are only 2 sections long)
#define WHEEL_LEN (SECTION_LEN*5)   // number of pixels in one side of the wheel (top or bottom)


DMAMEM int displayMemory[STRIP_LEN*6];
int drawingMemory[STRIP_LEN*6];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(STRIP_LEN, displayMemory, drawingMemory, config);

int temps[WHEEL_LEN];

// Map wheel idx to led num 
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



int pos;
int v;

void setup() {
    pinMode(1, OUTPUT);
    digitalWrite(1, HIGH);
    for (int i=0; i<WHEEL_LEN; i++) {
        temps[i] = 1000.0;
    }
    pos = WHEEL_LEN;
    v = 1;
    digitalWrite(1, LOW);
    leds.begin();
}


void loop() {

    int i;
    for (i=0; i<v; i++) {
        // Update position
        pos--;
        if (pos >= 0) {
            temps[pos] += 7500;
        }
        if (pos <= 0) {
            pos = WHEEL_LEN;
            v++;
            if (v > 10) v = 1;
        }
    }

    // Write colors
    digitalWrite(1, HIGH);
    for (i=0; i < WHEEL_LEN; i++) {
        float t = temps[i];
        int color = blackBodyColor(t);
        leds.setPixel(wheel_idx_to_led_num(i, true), color);
        leds.setPixel(wheel_idx_to_led_num(i, false), color);
        //leds.setPixel(i+j*5*144, color);
        temps[i]*=0.8;
    }
    leds.show();
    digitalWrite(1, LOW);
    delayMicroseconds(30*1000);
}

