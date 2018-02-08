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

// structures
struct IntLeftRight {
    int left;
    int right;
};

struct Log {
    struct IntLeftRight ticks;
    struct IntLeftRight speed;
};

// global variables
struct Log logs[199];
struct IntLeftRight preTicks = {
        .left = 0,
        .right = 0
};
struct IntLeftRight preSpeed = {
        .left = 64,
        .right = 64
};
struct IntLeftRight newSpeed = {
        .left = 64,
        .right = 64
};
int logCounter = 0;


void updateLog() {
// log speed
    logs[logCounter].speed.left = preSpeed.left;
    logs[logCounter].speed.right = preSpeed.right;

    preSpeed.left = newSpeed.left;
    preSpeed.right = newSpeed.right;
    drive_speed(preSpeed.left, preSpeed.right);

    drive_getTicks(&logs[logCounter].ticks.left, &logs[logCounter].ticks.right);
    // calculate the difference in ticks
    logs[logCounter].ticks.left = logs[logCounter].ticks.left - preTicks.left;
    logs[logCounter].ticks.right = logs[logCounter].ticks.right - preTicks.right;
    // update pre-ticks
    preTicks.left = preTicks.left + logs[logCounter].ticks.left;
    preTicks.right = preTicks.right + logs[logCounter].ticks.right;

    printf("Travelled for (%d, %d) in speed (%d, %d)\n\n", logs[logCounter].ticks.left, logs[logCounter].ticks.right,
           logs[logCounter].speed.left, logs[logCounter].speed.right);

    logCounter++;
}

int main() {
    int preLeftDis = 0;
    int newLeftDis = leftDis();

    drive_speed(preSpeed.left, preSpeed.right);

    while (ping_cm(8) > 20) {
        int needRecord = 0;
        preLeftDis = newLeftDis;
        newLeftDis = leftDis();
        int leftDisChange = newLeftDis - preLeftDis;

        printf("Change in left distance is: %d\n", leftDisChange);

        if (leftDisChange > 10) {
            newSpeed.left = 16;
            newSpeed.right = 64;
        } else if (leftDisChange > 5) {
            newSpeed.left = 24;
            newSpeed.right = 64;
        } else if (leftDisChange > 3) {
            newSpeed.left = 32;
            newSpeed.right = 64;
        } else if (leftDisChange > -3) {
            newSpeed.left = 64;
            newSpeed.right = 64;
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

        printf("new speed is: (%d, %d)\n", newSpeed.left, newSpeed.right);

        if (preSpeed.left != newSpeed.left || preSpeed.right != newSpeed.right) { // need to update speed and record
            updateLog();
        }
        pause(50);
    }
    // stop and pause to ensure that bot has fully stopped
    drive_speed(0, 0);

    pause(1000);
    updateLog();

    // calculate position
    double x = 0.0;
    double y = 0.0;
    double theta = 0.0;
    double BOT_WIDTH = 32.5538;

    for (int i = 0; i < logCounter; i++) {
        double currentDegree = (logs[i].ticks.left - logs[i].ticks.right) / BOT_WIDTH;
        theta = theta + currentDegree;
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


    drive_goto(51, -51);
    pause(2000);

    // go back
    printf("********HEADING BACK********");
    struct IntLeftRight newTicks;
    // update history ticks
    logCounter--;
    drive_getTicks(&preTicks.left, &preTicks.right);
    // when going back, left and right need to be reversed
    if (logs[logCounter].ticks.left == logs[logCounter].ticks.right) {
        drive_speed(64, 64);
        preSpeed.right = 64;
        preSpeed.left = 64;
    } else {
        preSpeed.right = logs[logCounter].speed.right;
        preSpeed.left = logs[logCounter].speed.left;
    }
    drive_speed(preSpeed.left, preSpeed.right);
    while (1) {
        drive_getTicks(&newTicks.left, &newTicks.right);
        if (newTicks.left - preTicks.left >= logs[logCounter].ticks.right &&
            newTicks.right - preTicks.right >= logs[logCounter].ticks.left) {
            // ticks recorded in log have been traveled
            printf("Travelled for (%d, %d) in speed (%d, %d)\n\n", newTicks.left - preTicks.left,
                   newTicks.right - preTicks.right,
                   preSpeed.left, preSpeed.right);
            if (logCounter == 0) {
                break;
            }
            logCounter--;
            drive_getTicks(&preTicks.left, &preTicks.right);
            if (logs[logCounter].ticks.left == logs[logCounter].ticks.right) {
                drive_speed(64, 64);
                preSpeed.right = 64;
                preSpeed.left = 64;
            } else {
                preSpeed.right = logs[logCounter].speed.right;
                preSpeed.left = logs[logCounter].speed.left;
            }
            drive_speed(preSpeed.left, preSpeed.right);
        }
    }
    drive_speed(0, 0);

    return 0;
}
