#include <GL/gl.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "OctoWS2811.h"
#include "wheel.h"
#include "makeColor.h"

#ifndef SIM_NAME
#define SIM_NAME neutrinos
#endif


#define CATFN_H(a, b) a##b
#define CATFN(a, b) CATFN_H(a, b)

void CATFN(loop_, SIM_NAME)();
void CATFN(setup_, SIM_NAME)();

//void loop_ ## SIM_NAME();
//void setup_ ## SIM_NAME();


OctoWS2811 leds(2*WHEEL_LEN, NULL, NULL, 0);
// Initialized in sim.c
int rainbowColors[180];

int last_delay;
//OctoWS2811 leds(2*WHEEL_LEN, NULL, NULL, 0);

void delay(size_t ms)
{
    /*struct timespec tm;
    tm.tv_sec = (ms / 1000);
    tm.tv_nsec = ((ms % 1000) * 1000000);
    nanosleep(&tm, NULL);*/
    last_delay = ms;
}

void delayMicroseconds(size_t us)
{
    delay(us/1000);
}

int wheel_idx_to_led_num(int idx, bool is_top)
{
    return idx + (is_top ? 0 : WHEEL_LEN);
}

void glut_setup()
{
    //glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClearColor(0.05, 0.05, 0.05, 1.0);
}

void drawFilledCircle(GLfloat x, GLfloat y, GLfloat radius){
    int i;
    int triangleAmount = 20; //# of triangles used to draw circle

    //GLfloat radius = 0.8f; //radius
    GLfloat twicePi = 2.0f * M_PI;

    glBegin(GL_TRIANGLE_FAN);
        //glVertex2f(x, y); // center of circle
        for(i = 0; i <= triangleAmount;i++) {
            glVertex2f(
                x + (radius * cos(i *  twicePi / triangleAmount)),
                y + (radius * sin(i * twicePi / triangleAmount))
            );
        }
    glEnd();
}

double aspect = 1.0;
//(glutGet(GLUT_WINDOW_WIDTH)/glutGet(GLUT_WINDOW_HEIGHT));
// Draw the wheel!
void display()
{
    //gluPerspective(0.0, aspect, 0.0, 100000.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    int num_leds = WHEEL_LEN;
    double radius = 0.95;
    GLfloat twicePi = 2.0*M_PI;


    for (int i=0; i<num_leds; i++) {
        int color = leds.getPixel(i);
        double color_r = ((float)((color >> 16) & 0xFF)) / 255.0;
        double color_g = ((float)((color >> 8) & 0xFF)) / 255.0;
        double color_b = ((float)((color >> 0) & 0xFF)) / 255.0;
        glColor4f(color_r, color_g, color_b, 1.0);
        if (color != 0) {
            drawFilledCircle(
                radius*cos(i*twicePi / num_leds),
                radius*sin(i*twicePi / num_leds),
                0.005f);
        }
    }

    glutSwapBuffers();
}

void drive (int data) {
    // callback function moves the car.
    //   .... move x, y, z, etc
    CATFN(loop_, SIM_NAME)();
    //loop_neutrinos();
    glutTimerFunc(last_delay, drive, -1); // call drive() again in 30 milliseconds
    glutPostRedisplay();
}


void resize(int width, int height) {
    // we ignore the params and do:

    //glutReshapeWindow(1000,1000);
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize(1040,1040);
    glMatrixMode(GL_PROJECTION);
    glutCreateWindow("LED Cyr Wheel");

    for (int i=0; i<180; i++) {
        rainbowColors[i] = makeColor(i*2, 100, 20);
    }

    CATFN(setup_, SIM_NAME)();
    last_delay = 30;

    glut_setup();
    glutReshapeFunc(resize);
    glutDisplayFunc(display);
    glutTimerFunc(30, drive, -1); // physics, movement equations here
    glutMainLoop();
    return 0;
}
