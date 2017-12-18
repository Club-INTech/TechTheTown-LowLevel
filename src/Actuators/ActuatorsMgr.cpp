// 
// 
// 

#include "ActuatorsMgr.h"

ActuatorsMgr::ActuatorsMgr() : dynamixelMgr(DynamixelMgr::Instance())
{
}

ActuatorsMgr::~ActuatorsMgr()
{
}


void ActuatorsMgr::movAX12(int id, uint16_t angleDegree)
{
	dynamixelMgr.movAX12(id, angleDegree);
}
void ActuatorsMgr::setAX12Speed(int id, uint16_t speed)
{
	dynamixelMgr.setAX12Speed(id, speed);
}

void ActuatorsMgr::movAX12G(unsigned int groupId, uint16_t angleDegree)
{
	dynamixelMgr.movAX12G(groupId, angleDegree);
}

void ActuatorsMgr::setAX12GSpeed(unsigned int groupId, uint16_t speed)
{
	dynamixelMgr.setAX12GSpeed(groupId, speed);
}

void ActuatorsMgr::setPumpState(bool newState)
{
    if (newState)
    {
        analogWrite(PIN_PWM_POMPE,64);
    }
    else
    {
        analogWrite(PIN_PWM_POMPE,0);
    }
}
