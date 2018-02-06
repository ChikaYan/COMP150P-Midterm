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

struct IntLeftRight {
    int left;
    int right;
};

struct Log {
    struct IntLeftRight ticks;
    struct IntLeftRight speed;
};


int main() {
    struct Log logs[199];
    struct IntLeftRight preTicks = {
            .left = 0,
            .right = 0
    };
    struct IntLeftRight preSpeed = {
            .left = 64,
            .right = 64
    };
    int logCounter = 0;

    int preLeftDis = 0;
    int newLeftDis = leftDis();
    drive_speed(preSpeed.left, preSpeed.right);
    while (ping_cm(8) > 12) {
        int needRecord = 0;
        struct IntLeftRight newSpeed = {
                .left = 0,
                .right = 0
        };


        preLeftDis = newLeftDis;
        newLeftDis = leftDis();

        int leftDisChange = newLeftDis - preLeftDis;

        if (leftDisChange > 10) {
            newSpeed.left = 16;
            newSpeed.right = 64;
        } else if (leftDisChange > 5) {
            newSpeed.left = 24;
            newSpeed.right = 64;
        } else if (leftDisChange > 3) {
            newSpeed.left = 32;
            newSpeed.right = 64;
        } else if (leftDisChange > 0) {
            newSpeed.left = 48;
            newSpeed.right = 64;
        } else if (leftDisChange == 0) {
            newSpeed.left = 64;
            newSpeed.right = 64;
        } else if (leftDisChange > -3) {
            newSpeed.left = 64;
            newSpeed.right = 48;
        } else if (leftDisChange > -6) {
            newSpeed.left = 64;
            newSpeed.right = 32;
        } else if (leftDisChange > -11) {
            newSpeed.left = 64;
            newSpeed.right = 24;
        } else {
            newSpeed.left = 64;
            newSpeed.right = 16;
        }

        if (preSpeed.left != newSpeed.left || preSpeed.right != newSpeed.right) { // need to update speed and record
            preSpeed.left = newSpeed.left;
            preSpeed.right = newSpeed.right;
            drive_speed(preSpeed.left, preSpeed.right);
            // log speed
            logs[logCounter].speed.left = preSpeed.left;
            logs[logCounter].speed.right = preSpeed.right;

            drive_getTicks(&logs[logCounter].ticks.left, &logs[logCounter].ticks.right);
            // calculate the difference in ticks
            logs[logCounter].ticks.left = logs[logCounter].ticks.left - preTicks.left;
            logs[logCounter].ticks.right = logs[logCounter].ticks.right - preTicks.right;
            // update pre-ticks
            preTicks.left = preTicks.left + logs[logCounter].ticks.left;
            preTicks.right = preTicks.right + logs[logCounter].ticks.right;
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
        double currentD = (logs[i].ticks.left - logs[i].ticks.right) / BOT_WIDTH;
        theta = theta + currentD;
        if (theta != 0) {
            double rl = logs[i].ticks.left / theta;
            double rr = logs[i].ticks.right / theta;
            double rm = (rl + rr) / 2.0;
            x = x + rm - rm * cos(theta);
            y = y + rm * sin(theta);
        } else {
            x = x + logs[i].ticks.left;
            y = y + logs[i].ticks.right;
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
//    for (int i = counter - 1; i >= 0; i--) {
//
//    }

    return 0;
}



