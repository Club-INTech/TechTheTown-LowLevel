//
// Created by trotfunky on 26/11/18.
//

#ifndef LL_ROBOTSTATUS_H
#define LL_ROBOTSTATUS_H

#include "Encoder.h"

enum class MOVEMENT { FORWARD, BACKWARD, TRIGO, ANTITRIGO, CURVE, NONE };

class RobotStatus {

private:
    Encoder leftEncoder;
    Encoder rightEncoder;

public:
    bool controlled;
    bool pTpControl;
    bool forcedMovement;
    bool moving;
    bool stuck;

    int16_t x;
    int16_t y;
    float orientation;

    MOVEMENT movement;

    float speedTranslation;
    float speedRotation;
    float speedLeftWheel;
    float speedRightWheel;

    RobotStatus();
    void updateStatus();
};

#endif //LL_ROBOTSTATUS_H
