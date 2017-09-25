#include <OctoWS2811.h>
#include "wheel.h"

DMAMEM int displayMemory[STRIP_LEN * 12];
int drawingMemory[STRIP_LEN * 12];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(STRIP_LEN, displayMemory, drawingMemory, config);


struct mixer_pixel {
  int r;
  int g;
  int b;
  int n;  // number
};

struct mixer_pixel mixer_pixs[WHEEL_LEN];

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

void blue(int len)
{

  for (int j = 0; j < len; j++) {
    for (int i = 0; i < WHEEL_LEN; i++) {
      leds.setPixel(wheel_idx_to_led_num(i, true), 0x0000FF);
      leds.setPixel(wheel_idx_to_led_num(i, false), 0x0000FF);
    }
    leds.show();
    delayMicroseconds(50 * 1000);
    for (int i = 0; i < WHEEL_LEN; i++) {
      leds.setPixel(wheel_idx_to_led_num(i, true), 0x00);
      leds.setPixel(wheel_idx_to_led_num(i, false), 0x00);
    }
    leds.show();
    delayMicroseconds(50 * 1000);
  }
  delayMicroseconds(500 * 1000);
}

void mixer_reset()
{
  memset(mixer_pixs, 0, sizeof(struct mixer_pixel)*WHEEL_LEN);
}

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

#define MAX_LINES 1000

int rainbowColors[180];
struct Line lines[MAX_LINES];

#define DECAY_START 30

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
  if (line->life < 0) line->life = 0;
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
    mixer_add(x, (line->r * scale) / 100, (line->g * scale) / 100, (line->b * scale) / 100);
    x = (x + 1) % WHEEL_LEN;
  }
}


int time;

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
  //int speed = ((time / 3) % 400) - 200;
  int color = rainbowColors[(time / 2) % 180];
  line->start = (rand() % (100 * WHEEL_LEN));
  line->end = line->start + 100 * ((rand() % 2) + 1);
  line->speed = (rand() % 400) - 200;// - 200;
  line->life = (rand() % 80) + 2;
  line->r = (color >> 16) & 0xFF;
  line->g = (color >> 8) & 0xFF;
  line->b = (color) & 0xFF;

  //blue(rand() % 100);
}


void setup() {

  srand(0xdeadbeef);
  for (int i = 0; i < 180; i++) {
    int hue = i * 2;
    int saturation = 100;
    int lightness = 20;
    // pre-compute the 180 rainbow colors
    rainbowColors[i] = makeColor(hue, saturation, lightness);
  }
  memset(lines, 0, sizeof(struct Line)*MAX_LINES);
  time = 0;
  digitalWrite(1, LOW);
  leds.begin();
}

void loop() {


  // put your main code here, to run repeatedly:
  mixer_reset();
  for (int i = 0; i < MAX_LINES; i++) {
    move_line(&lines[i]);
    draw_line(&lines[i]);
  }
  
  Serial.println("Drew lines...");
  Serial.flush();
  time++;
  new_line();
  new_line();
  mixer_blend();
  leds.show();

  delayMicroseconds(25 * 1000);
}
