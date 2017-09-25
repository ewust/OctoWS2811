#include <OctoWS2811.h>
#include "wheel.h"

// OctoWS2811 globals/scratch space
DMAMEM int displayMemory[STRIP_LEN * 12];
int drawingMemory[STRIP_LEN * 12];

// Globals for shows
OctoWS2811 leds(STRIP_LEN, displayMemory, drawingMemory, WS2811_GRB | WS2811_800kHz);
int rainbowColors[180]; // precomputed HSV -> RGB table


// Time for each show in milliseconds
#define SHOW_RUNTIME 30000

struct config {
    void (**loop_fns)(void);
    int cur_loop_fn_idx;
    int show_start_time;
};

struct config conf;

void setup() {

    while (!Serial && millis() < 500);
    Serial.begin(38400);

    Serial.println("setup");
    Serial.flush();

    // Initialize config
    conf.loop_fns = (void (**)(void))calloc(sizeof(void (*)(void)), 3);
    conf.loop_fns[0] = &loop_neutrinos;
    conf.loop_fns[1] = &loop_blazers;
    conf.loop_fns[2] = NULL;

    conf.cur_loop_fn_idx = 0;
    conf.show_start_time = millis();

    // Things used by multiple shows should be setup here
    for (int i = 0; i < 180; i++) {
        int hue = i * 2;
        int saturation = 100;
        int lightness = 20;
        // pre-compute the 180 rainbow colors
        rainbowColors[i] = makeColor(hue, saturation, lightness);
    }

    // Call show-specific setups
    setup_neutrinos();
    setup_blazers();

    leds.begin();

    Serial.println("setup done");
    Serial.flush();
}


void loop() {
    if ((millis() - conf.show_start_time) > SHOW_RUNTIME) {
        // Next show
        conf.cur_loop_fn_idx++;
        if (conf.loop_fns[conf.cur_loop_fn_idx] == NULL) {
            conf.cur_loop_fn_idx = 0;
        }

        conf.show_start_time = millis();
    }

    // Call the current show
    conf.loop_fns[conf.cur_loop_fn_idx]();
}
