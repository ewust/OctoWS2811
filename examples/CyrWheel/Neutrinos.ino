#include <OctoWS2811.h>
#include "wheel.h"
#include <stdlib.h>


#define MAX_LINES 1000
#define DECAY_START 30


struct mixer_pixel {
    int r;
    int g;
    int b;
    int n;  // number
};

struct Line {
    // use fixed point 1/100th of a pixel
    int start;  // start position of line, times 100
    int end;    // end position, times 100
    int speed;  // cent-speed (divide this by 100 to get true speed (float))
    int life;
    int r;
    int g;
    int b;
};


struct mixer_pixel mixer_pixs[WHEEL_LEN];
struct Line lines[MAX_LINES];
int neutrino_time;

void mixer_add(int x, int r, int g, int b)
{
    mixer_pixs[x].r += r;
    mixer_pixs[x].g += g;
    mixer_pixs[x].b += b;
    mixer_pixs[x].n++;
}

void mixer_blend() {
    for (int i = 0; i < WHEEL_LEN; i++) {
        int n = mixer_pixs[i].n;
        if (n == 0) n = 1;
        int r = (mixer_pixs[i].r / n) & 0xFF;
        int g = (mixer_pixs[i].g / n) & 0xFF;
        int b = (mixer_pixs[i].b / n) & 0xFF;
        int color = (r << 16) | (g << 8) | (b);
        //color = 0x000088;
        leds.setPixel(wheel_idx_to_led_num(i, true), color);
        leds.setPixel(wheel_idx_to_led_num(i, false), color);
    }
}

void mixer_reset()
{
    memset(mixer_pixs, 0, sizeof(struct mixer_pixel)*WHEEL_LEN);
}

void move_line(struct Line *line)
{
    line->start = (line->start + line->speed) % (100 * WHEEL_LEN);
    if (line->start < 0) {
        line->start += 100 * WHEEL_LEN;
    }
    line->end = (line->end + line->speed) % (100 * WHEEL_LEN);
    if (line->end < 0) {
        line->end += 100 * WHEEL_LEN;
    }
    line->life--;
    if (line->life < 0) {
        line->life = 0;
    }
}

void draw_line(struct Line *line)
{
    if (line->life == 0) {
        return;
    }
    int x = line->start / 100;
    int end_x = line->end / 100;
    while (x != end_x) {
        int scale = 100;
        if (line->life < DECAY_START) {
            scale = (line->life * 100) / DECAY_START;
        }
        mixer_add(x,
                (line->r * scale) / 100,
                (line->g * scale) / 100,
                (line->b * scale) / 100);
        x = (x + 1) % WHEEL_LEN;
    }
}



void new_line()
{
    struct Line *line = NULL;
    for (int i = 0; i < MAX_LINES; i++) {
        if (lines[i].life == 0) {
            line = &lines[i];
            break;
        }
    }
    if (!line) {
        return;
    }
    //int speed = ((neutrino_time / 3) % 400) - 200;
    int color = rainbowColors[(neutrino_time / 2) % 180];
    line->start = (rand() % (100 * WHEEL_LEN));
    line->end = line->start + 100 * ((rand() % 2) + 1);
    line->speed = (rand() % 400) - 200;// - 200;
    line->life = (rand() % 80) + 2;
    line->r = (color >> 16) & 0xFF;
    line->g = (color >> 8) & 0xFF;
    line->b = (color) & 0xFF;
}


void setup_neutrinos() {

    srand(0xdeadbeef);
    memset(lines, 0, sizeof(struct Line)*MAX_LINES);
    neutrino_time = 0;
}

void loop_neutrinos() {
    mixer_reset();
    for (int i = 0; i < MAX_LINES; i++) {
        move_line(&lines[i]);
        draw_line(&lines[i]);
    }

    neutrino_time++;
    new_line();
    new_line();
    mixer_blend();
    leds.show();

    delayMicroseconds(25 * 1000);
}
