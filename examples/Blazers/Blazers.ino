#include <OctoWS2811.h>
#include "wheel.h"





//#define SECTION_LEN 178             // number of pixels in one section
//#define STRIP_LEN (SECTION_LEN)   // longest strip length. Pretend all are equal (even though some are only 2 sections long)
//#define WHEEL_LEN (SECTION_LEN*5)   // number of pixels in one side of the wheel (top or bottom)


DMAMEM int displayMemory[STRIP_LEN*12];
int drawingMemory[STRIP_LEN*12];
//uint16_t *drawingMemory = displayMemory;

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(SECTION_LEN, displayMemory, drawingMemory, config);

int temps[WHEEL_LEN];

// Map wheel idx to led num 
/*
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
          num += (SECTION_LEN - idx - 1);
        } else if (idx < 2*SECTION_LEN) {
          num += (3*SECTION_LEN - idx - 1);
        } else {
          num += (5*SECTION_LEN - idx - 1);
        }
    }
    return num;
}
*/




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



  while (!Serial && (millis() < 2000)) ;
  Serial.begin(38400);

    Serial.println("Setup Called...");
    Serial.flush();
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


  Serial.println("Loop Called...");
  Serial.flush();
    int i;
    for (i=0; i<v; i++) {
        // Update position
        pos++;
        if (pos >= 0 && pos < WHEEL_LEN) {
            temps[pos] += 7500;
        }
        if (pos > WHEEL_LEN) {
            pos = 0;
            v++;
            if (v > 10) v = 1;
        }
    }

    // Write colors
    //digitalWrite(1, HIGH);
    for (i=0; i < WHEEL_LEN; i++) {
        float t = temps[i];
        int color = blackBodyColor(t);
        leds.setPixel(wheel_idx_to_led_num(i, true), color);
        leds.setPixel(wheel_idx_to_led_num(i, false), color);
        //leds.setPixel(i+j*5*144, color);
        temps[i]*=0.8;
    }
    leds.show();
    //digitalWrite(1, LOW);
    delayMicroseconds(30*1000);
}

