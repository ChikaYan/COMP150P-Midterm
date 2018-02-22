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

struct floatLeftRight {
    float left;
    float right;
};

struct Log {
    struct IntLeftRight ticks;
    struct IntLeftRight speed;
};

// global variables
struct Log logs[199];
int logCounter = 0;
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
const int INIT_SPEED = 64;

// PID parameters
const float THRESHOLD = 2.25;
const float KP = 3.8;
const float KI = 2.25;
const float KD = 0.7;

struct floatLeftRight integral = {
        .left = 0,
        .right = 0
};
struct floatLeftRight lastError = {
        .left = 0,
        .right = 0
};


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

void P_controller(int leftDisChange, int rightDisChange, int initial_speed, float P_value, int tolerance) {
    if (leftDisChange - rightDisChange > tolerance) {
        newSpeed.left = round(initial_speed * (leftDisChange - rightDisChange) * P_value);
        newSpeed.right = initial_speed;
    } else if (rightDisChange - leftDisChange > tolerance) {
        newSpeed.left = initial_speed;
        newSpeed.right = round(initial_speed * (rightDisChange - leftDisChange) * P_value);
    } else {
        newSpeed.left = initial_speed;
        newSpeed.right = initial_speed;
    }
}

int pidControllerLeft(float disChangeLeft) {
    float error = disChangeLeft - 0; // desired distance change is 0
    if (abs(error) >= THRESHOLD) {
        integral.left = 0;
    } else {
        integral.left = integral.left + error;
    }
    float derivative = error - lastError.left;
    lastError.left = error;
    printf("ERROR: %f, INTEGRAL: %f, DERIVATIVE: %f\n", error, integral.left, derivative);
    return round(error * KP + integral.left * KI + derivative * KD);
}


int pidControllerRight(float disChangeRight) {
    float error = disChangeRight - 0; // desired distance change is 0
    if (abs(error) >= THRESHOLD) {
        integral.right = 0;
    } else {
        integral.right = integral.right + error;
    }
    float derivative = error - lastError.right;
    lastError.right = error;
    printf("ERROR: %f, INTEGRAL: %f, DERIVATIVE: %f\n", error, integral.right, derivative);
    return round(error * KP + integral.right * KI + derivative * KD);
}

void takeSpeedFromLog() {
    logCounter--;
    if (logs[logCounter].ticks.left == logs[logCounter].ticks.right) {
        preSpeed.right = INIT_SPEED;
        preSpeed.left = INIT_SPEED;
    } else if (logs[logCounter].ticks.left < logs[logCounter].ticks.right) {
        preSpeed.right = round((float) logs[logCounter].ticks.left / (float) logs[logCounter].ticks.right * INIT_SPEED);
        preSpeed.left = INIT_SPEED;
    } else {
        preSpeed.right = INIT_SPEED;
        preSpeed.left = round((float) logs[logCounter].ticks.right / (float) logs[logCounter].ticks.left * INIT_SPEED);
    }
    drive_speed(preSpeed.left, preSpeed.right);
}

int main() {
    struct floatLeftRight preDis = {
            .left = 0,
            .right = 0
    };
    struct floatLeftRight newDis = {
            .left = leftDis(),
            .right = rightDis()
    };

    drive_speed(preSpeed.left, preSpeed.right);
    while (ping_cm(8) > 20) {
        preDis.left = newDis.left;
        preDis.right = newDis.right;
        newDis.left = leftDis();
        newDis.right = rightDis();

        struct floatLeftRight disChange = {
                .left =newDis.left - preDis.left,
                .right = newDis.right - preDis.right
        };

//        printf("Change in left distance is: %d\n", leftDisChange);
//        printf("Change in right distance is: %d\n", rightDisChange);
        //P_controller(leftDisChange, rightDisChange, 64, 0.025, 1);
        struct IntLeftRight pidValue = {
                .left = pidControllerLeft(disChange.left),
                .right = pidControllerRight(disChange.right)
        };
        printf("Change in left distance is: %f\nPID value is: %d\n", disChange.left, pidValue.left);
        printf("Change in right distance is: %f\nPID value is: %d\n", disChange.right, pidValue.right);
        newSpeed.left = INIT_SPEED - pidValue.left;
        newSpeed.right = INIT_SPEED - pidValue.right;

//        if (pidValue > 0) {
//            newSpeed.left = INIT_SPEED - pidValue;
//            newSpeed.right = INIT_SPEED;
//        } else {
//            newSpeed.left = INIT_SPEED;
//            newSpeed.right = INIT_SPEED + pidValue;
//        }

        //printf("new speed is: (%d, %d)\n", newSpeed.left, newSpeed.right);
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

    // turning
    drive_goto(-2, -2);
    if (leftDis() >= rightDis()) {
        drive_goto(51, -51);
    } else {
        drive_goto(-51, 51);
    }

    pause(2000);

    // go back
    printf("********HEADING BACK********\n");
    struct IntLeftRight newTicks;
    // update history ticks

    drive_getTicks(&preTicks.left, &preTicks.right);
    // when going back, left and right need to be reversed
    takeSpeedFromLog();
    while (1) {
        drive_getTicks(&newTicks.left, &newTicks.right);
        if (newTicks.left - preTicks.left >= logs[logCounter].ticks.right &&
            newTicks.right - preTicks.right >= logs[logCounter].ticks.left) {
            // ticks recorded in log have been traveled
            printf("Travelled for (%d, %d) in speed (%d, %d)\nLog is: Travelled for (%d, %d) in speed (%d, %d)\n\n",
                   newTicks.left - preTicks.left,
                   newTicks.right - preTicks.right,
                   preSpeed.left, preSpeed.right, logs[logCounter].ticks.left, logs[logCounter].ticks.right,
                   logs[logCounter].speed.left, logs[logCounter].speed.right);

            if (logCounter == 0) {
                break;
            }
            drive_getTicks(&preTicks.left, &preTicks.right);
            takeSpeedFromLog();
        }
    }
    drive_goto(50, 50);
    return 0;
}