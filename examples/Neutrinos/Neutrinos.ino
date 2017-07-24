#include <OctoWS2811.h>
#define SECTION_LEN 178             // number of pixels in one section
#define STRIP_LEN (SECTION_LEN)   // longest strip length. Pretend all are equal (even though some are only 2 sections long)
#define WHEEL_LEN (SECTION_LEN*5)   // number of pixels in one side of the wheel (top or bottom)


DMAMEM uint16_t displayMemory[SECTION_LEN*24];
uint16_t drawingMemory[SECTION_LEN*24];
//uint16_t *drawingMemory = displayMemory;

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(SECTION_LEN, displayMemory, drawingMemory, config);

int temps[WHEEL_LEN];

// Map wheel idx to led num 

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
  for (int i=0; i<WHEEL_LEN; i++) {
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

  for (int j=0; j<len; j++) {
  for (int i=0; i<WHEEL_LEN; i++) {
    leds.setPixel(wheel_idx_to_led_num(i, true), 0x0000FF);
    leds.setPixel(wheel_idx_to_led_num(i, false), 0x0000FF);
  }
  leds.show();
  delayMicroseconds(50*1000);
  for (int i=0; i<WHEEL_LEN; i++) {
    leds.setPixel(wheel_idx_to_led_num(i, true), 0x00);
    leds.setPixel(wheel_idx_to_led_num(i, false), 0x00);
  }
  leds.show();
  delayMicroseconds(50*1000);
  }
  delayMicroseconds(500*1000);
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

#define MAX_LINES 500

int rainbowColors[180];
struct Line lines[MAX_LINES];

#define DECAY_START 30

void move_line(struct Line *line)
{
  line->start = (line->start + line->speed) % (100*WHEEL_LEN);
  if (line->start < 0) {
    line->start += 100*WHEEL_LEN;
  }
  line->end = (line->end + line->speed) % (100*WHEEL_LEN);
  if (line->end < 0) {
    line->end += 100*WHEEL_LEN;
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
      scale = (line->life*100) / DECAY_START;
    }
    mixer_add(x, (line->r*scale)/100, (line->g*scale)/100, (line->b*scale)/100);
    x = (x + 1) % WHEEL_LEN;
  }
}


int time;

void new_line()
{
  struct Line *line = NULL;
  for (int i=0; i<MAX_LINES; i++) {
    if (lines[i].life == 0) {
      line = &lines[i];
      break;
    }
  }
  if (!line) {
    return;
  }
  int color = rainbowColors[(time/2) % 180];
  line->start = (rand() % (100*WHEEL_LEN));
  line->end = line->start + 100*((rand() % 2) + 1);
  line->speed = (rand() % 400) - 200;
  line->life = (rand() % 80) + 2;
  line->r = (color >> 16) & 0xFF;
  line->g = (color >> 8) & 0xFF;
  line->b = (color) & 0xFF;

  //blue(rand() % 100);
}


void setup() {
  pinMode(1, OUTPUT);
  digitalWrite(1, HIGH);
  srand(0xdeadbeef);
  for (int i=0; i<180; i++) {
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
  for (int i=0; i<MAX_LINES; i++) {
    move_line(&lines[i]);
    draw_line(&lines[i]);
  }
  time++;
  new_line();
  mixer_blend();
  leds.show();
  
  delayMicroseconds(25*1000);
}
