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

int leftDis() {
    int init = 0;
    for (int dacVal = 0; dacVal < 160; dacVal += 4)  // <- add
    {                                               // <- add
        dac_ctr(26, 0, dacVal);                       // <- add
        freqout(11, 1, 38000);                        // <- add
        init += input(10);
    }
    return init;
}

#endif //PROJECT_BASICMOVE_H
