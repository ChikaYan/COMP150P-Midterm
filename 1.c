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

struct Log {
    int ticksL;
    int ticksR;
    int speedL;
    int speedR;
};


int main() {
    Log logs[199];
    int logCounter = 0, preLeftTicks = 0, preRightTicks = 0, preLeftSpeed = 64, preRightSpeed = 64;
    int preLeftDis = 0;
    int newLeftDis = leftDis();
    drive_speed(preLeftSpeed, preRightSpeed);
    while (ping_cm(8) > 12) {
        int needRecord = 0;
        int newLS = 0, newRS = 0;

        preLeftDis = newLeftDis;
        newLeftDis = leftDis();

        int leftDisChange = newLeftDis - preLeftDis;

        if (leftDisChange > 10) {
            newLS = 16;
            newRS = 64;
        } else if (leftDisChange > 5) {
            newLS = 24;
            newRS = 64;
        } else if (leftDisChange > 3) {
            newLS = 32;
            newRS = 64;
        } else if (leftDisChange > 0) {
            newLS = 48;
            newRS = 64;
        } else if (leftDisChange == 0) {
            newLS = 64;
            newRS = 64;
        } else if (leftDisChange > -3) {
            newLS = 64;
            newRS = 48;
        } else if (leftDisChange > -6) {
            newLS = 64;
            newRS = 32;
        } else if (leftDisChange > -11) {
            newLS = 64;
            newRS = 24;
        } else {
            newLS = 64;
            newRS = 16;
        }

        if (preLeftSpeed != newLS || preRightSpeed != newRS) { // need to update speed and record
            preLeftSpeed = newLS;
            preRightSpeed = newRS;
            drive_speed(preLeftSpeed, preRightSpeed);
            logs[logCounter].speedL = preLeftSpeed;
            logs[logCounter].speedR = preRightSpeed;
            drive_getTicks(&logs.ticksL[logCounter], &logs.ticksR[logCounter]);
            // calculate the difference in ticks
            logs.ticksL[logCounter] = logs.ticksL[logCounter] - preLeftTicks;
            logs.ticksR[logCounter] = logs.ticksR[logCounter] - preRightTicks;
            // update pre-ticks
            preLeftTicks = preLeftTicks + logs.ticksL[logCounter];
            preRightTicks = preRightTicks + logs.ticksR[logCounter];
            logCounter++;
        }

        pause(50);
    }

    // calculate position
    double x = 0.0;
    double y = 0.0;
    double theta = 0.0;
    double BOT_WIDTH = 32.5538;

    for (int i = 0; i < logCounter; i++) {
        double currentD = (logs.ticksL[i] - logs.ticksR[i]) / BOT_WIDTH;
        theta = theta + currentD;
        if (theta != 0) {
            double rl = logs.ticksL[i] / theta;
            double rr = logs.ticksR[i] / theta;
            double rm = (rl + rr) / 2.0;
            x = x + rm - rm * cos(theta);
            y = y + rm * sin(theta);
        } else {
            x = x + logs.ticksL[i];
            y = y + logs.ticksR[i];
        }

    }
    x = x * 0.325;
    y = y * 0.325;
    double distance = sqrt(x * x + y * y);
    printf("Degree: %f radius, Distance: %f cm\n", theta, distance);

    // Stop, turn and pause
    drive_speed(0, 0);
    drive_goto(51, -51);
    pause(200);

    // go back


    return 0;
}



