// ActuatorsMgr.h


#ifndef _ACTUATORSMGR_h
#define _ACTUATORSMGR_h

#include "Arduino.h"


#include "Dynamixel.h"
#include "DynamixelInterface.h"
#include "DynamixelMotor.h"
#include "Utils/Singleton.hpp"

class ActuatorsMgr : public Singleton<ActuatorsMgr>
{
private:
	DynamixelInterface serialAX;
	DynamixelMotor axTest;

public:
	ActuatorsMgr();

	~ActuatorsMgr();

	void testGoto(uint16_t);
};

#endif