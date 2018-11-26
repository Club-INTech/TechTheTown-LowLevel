//
// Created by trotfunky on 26/11/18.
//

#ifndef LL_MCSREBORN_H
#define LL_MCSREBORN_H

#include "Utils/Singleton.hpp"
#include "ControlSettings.h"
#include "RobotStatus.h"
#include "Motor.h"

// TODO : Tout docu

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

    void control();
    void stop();

    void translate(int16_t);
    void rotate(float);
    void gotoPoint(int16_t,int16_t,bool);

    void toggleControl();
    void toggleTranslation();
    void toggleRotation();

    int16_t getX();
    int16_t getY();
    float getAngle();

    void setX(int16_t);
    void setY(int16_t);
    void setAngle(float);
};

#endif //LL_MCSREBORN_H
