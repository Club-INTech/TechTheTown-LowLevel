// ActuatorsMgr.h

#ifndef _ACTUATORSMGR_h
#define _ACTUATORSMGR_h

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include "WProgram.h"
#endif

#include "Singleton.hpp"
#include "Dynamixel.h"
#include "DynamixelInterface.h"
#include "DynamixelMotor.h"

class ActuatorsMgr : public Singleton<ActuatorsMgr>
{
private:
	DynamixelInterface serialAX;
	DynamixelMotor* axTest;
public:
	ActuatorsMgr() : serialAX(DynamixelInterface(Serial1)){
		serialAX.begin(9600);
		axTest = new DynamixelMotor(serialAX, 0);
		axTest->init();
		axTest->jointMode();
		axTest->enableTorque();
	}

	~ActuatorsMgr() {
		delete axTest;
	}

	void testGoto(uint16_t goal) {
		axTest->goalPositionDegree(goal);
	}
};

#endif