// 
// 
// 

#include "ActuatorsMgr.h"

ActuatorsMgr::ActuatorsMgr() : dynamixelMgr(DynamixelMgr::Instance())
{
    isPumpOn = false;
    isElecVanneAVOpen = false;
    isElecVanneAROpen = false;
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

bool ActuatorsMgr::togglePumpState(bool newState)
{
    if (newState && (isElecVanneAVOpen+isElecVanneAROpen))
    {
        digitalWrite(PIN_PWM_POMPE,HIGH);
        isPumpOn = true;
        return(true);
    }
    else
    {
        digitalWrite(PIN_PWM_POMPE,LOW);
        isPumpOn = false;
        return(isElecVanneAVOpen+isElecVanneAROpen);
    }
}

void ActuatorsMgr::setElecVanneAV(bool newState)
{
    if (newState)
    {
        digitalWrite(PIN_ELECTROVANNE_AV,HIGH);
        isElecVanneAVOpen = true;
    }
    else
    {
        if(isPumpOn && !isElecVanneAROpen)
        {
            digitalWrite(PIN_PWM_POMPE,LOW);
            isPumpOn = false;
        }
        digitalWrite(PIN_ELECTROVANNE_AV,LOW);
        isElecVanneAVOpen = false;
    }
}
void ActuatorsMgr::setElecVanneAR(bool newState)
{
    if (newState)
    {
        digitalWrite(PIN_ELECTROVANNE_AR,HIGH);
        isElecVanneAROpen = true;
    }
    else
    {
        if(isPumpOn && !isElecVanneAVOpen)
        {
            digitalWrite(PIN_PWM_POMPE,LOW);
            isPumpOn = false;
        }
        digitalWrite(PIN_ELECTROVANNE_AR,LOW);
        isElecVanneAROpen = false;
    }
}
