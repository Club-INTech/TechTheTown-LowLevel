//
// Created by trotfunky on 26/11/18.
//

#ifndef LL_MCSREBORN_H
#define LL_MCSREBORN_H

#include "Utils/Singleton.hpp"
#include "ControlSettings.h"
#include "RobotStatus.h"
#include "Motor.h"

class MCS : public Singleton<MCS>
{

private:
    RobotStatus robotStatus;
    ControlSettings controlSettings;

    Motor leftMotor;
    Motor rightMotor;

    int16_t targetX;
    int16_t targetY;
    int16_t targetDistance;
    float targetAngle;

    float targetLeftSpeed;
    float targetRightSpeed;



public:

};

#endif //LL_MCSREBORN_H
