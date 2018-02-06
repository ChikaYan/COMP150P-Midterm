//
// Created by ChikamaYan on 1/29/2018.
//
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include "simpletools.h"
#include "abdrive.h"
#include "simpletools.h"
#include "ping.h"
#include "basicmove.h"

int main() {
    int ticksL[199], ticksR[199], counter = 0, preLT = 0, preRT = 0;
    int lastLeft = 0;
    int newLeft = leftDis();
    int preLS = 64, preRS = 64;
    drive_speed(preLS, preRS);
    while (ping_cm(8) > 12) {
        int needRecord = 0;
        int newLS = 0, newRS = 0;

        lastLeft = newLeft;
        newLeft = leftDis();

        int change = newLeft - lastLeft;

        if (change > 10) {
            newLS = 16;
            newRS = 64;
        } else if (change > 5) {
            newLS = 24;
            newRS = 64;
        } else if (change > 3) {
            newLS = 32;
            newRS = 64;
        } else if (change > 0) {
            newLS = 48;
            newRS = 64;
        } else if (change == 0) {
            newLS = 64;
            newRS = 64;
        } else if (change > -3) {
            newLS = 64;
            newRS = 48;
        } else if (change > -6) {
            newLS = 64;
            newRS = 32;
        } else if (change > -11) {
            newLS = 64;
            newRS = 24;
        } else {
            newLS = 64;
            newRS = 16;
        }

        if (preLS != newLS || preRS != newRS) { // need to update speed and record
            preLS = newLS;
            preRS = newRS;
            drive_speed(preLS, preRS);
            drive_getTicks(&ticksL[counter], &ticksR[counter]);
            //printf("before calc: %d %d\n", ticksL[counter], ticksR[counter]);
            // calculate the difference in ticks
            ticksL[counter] = ticksL[counter] - preLT;
            ticksR[counter] = ticksR[counter] - preRT;
            // update pre-ticks
            preLT = preLT + ticksL[counter];
            preRT = preRT + ticksR[counter];
            //printf("after calc: %d %d\n", ticksL[counter], ticksR[counter]);
            counter++;
        }
        pause(50);
    }
//    for (int i = 0; i < counter; i++) {
//        printf("%d %d\n", ticksL[i], ticksR[i]);
//    }

    // calculate position
    double x = 0.0;
    double y = 0.0;
    double theta = 0.0;
    double BOT_WIDTH = 32.5538;

    for (int i = 0; i < counter; i++) {
        double currentD = (ticksL[i] - ticksR[i]) / BOT_WIDTH;
        theta = theta + currentD;
        if (theta != 0) {
            double rl = ticksL[i] / theta;
            double rr = ticksR[i] / theta;
            double rm = (rl + rr) / 2.0;
            x = x + rm - rm * cos(theta);
            y = y + rm * sin(theta);
        } else {
            x = x + ticksL[i];
            y = y + ticksR[i];
        }

    }
    //printf("%f %f", x, y);
    x = x * 0.325;
    y = y * 0.325;
    double distance = sqrt(x * x + y * y);
    printf("Degree: %f radius, Distance: %f cm\n", theta, distance);


    // Stop
    drive_speed(0, 0);

    return 0;
}



