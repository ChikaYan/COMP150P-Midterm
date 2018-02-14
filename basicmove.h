//
// Created by ChikamaYan on 1/29/2018.
//

#ifndef PROJECT_BASICMOVE_H
#define PROJECT_BASICMOVE_H

#ifdef BUILDING_IN_SIMULATOR
#include "simulator.h"
#endif

void startTrail() {
#ifdef BUILDING_IN_SIMULATOR
    simulator_startNewSmokeTrail();
#endif
}

void stopTrail() {
#ifdef BUILDING_IN_SIMULATOR
    simulator_stopSmokeTrail();
#endif
}

void moveBot(int x) {
    drive_goto(x, x);
}

void turn(float a) {
    drive_goto(round(a * 0.284), round(a * (-0.284)));
}

float leftDis() {
    int init1 = 0, init2 = 0, init3 = 0, init4 = 0;
    for (int dacVal = 0; dacVal < 160; dacVal += 4) {
        dac_ctr(26, 0, dacVal);
        freqout(11, 1, 38000);
        init1 += input(10);
    }
    for (int dacVal = 0; dacVal < 160; dacVal += 4) {
        dac_ctr(26, 0, dacVal);
        freqout(11, 1, 38000);
        init2 += input(10);
    }
    for (int dacVal = 0; dacVal < 160; dacVal += 4) {
        dac_ctr(26, 0, dacVal);
        freqout(11, 1, 38000);
        init3 += input(10);
    }
    for (int dacVal = 0; dacVal < 160; dacVal += 4) {
        dac_ctr(26, 0, dacVal);
        freqout(11, 1, 38000);
        init4 += input(10);
    }
    return (init1 + init2 + init3 + init4) / 4.0;
}

float rightDis() {
    int init1 = 0, init2 = 0, init3 = 0, init4 = 0;
    for (int dacVal = 0; dacVal < 160; dacVal += 4) {
        dac_ctr(27, 1, dacVal);
        freqout(1, 1, 38000);
        init += input(2);
    }
    for (int dacVal = 0; dacVal < 160; dacVal += 4) {
        dac_ctr(27, 1, dacVal);
        freqout(1, 1, 38000);
        init2 += input(2);
    }
    for (int dacVal = 0; dacVal < 160; dacVal += 4) {
        dac_ctr(27, 1, dacVal);
        freqout(1, 1, 38000);
        init3 += input(2);
    }
    for (int dacVal = 0; dacVal < 160; dacVal += 4) {
        dac_ctr(27, 1, dacVal);
        freqout(1, 1, 38000);
        init4 += input(2);
    }
    return (init1 + init2 + init3 + init4) / 4.0;
}

#endif //PROJECT_BASICMOVE_H
