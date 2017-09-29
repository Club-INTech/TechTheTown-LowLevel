// ActuatorsMgr.h


#ifndef _ACTUATORSMGR_h
#define _ACTUATORSMGR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "wprogram.h"
#else
	#include "WProgram.h"
#endif


#include "Dynamixel.h"
#include "DynamixelInterface.h"
#include "DynamixelMotor.h"
#include "Singleton.hpp"

class ActuatorsMgr : public Singleton<ActuatorsMgr>
{
private:
	DynamixelInterface serialAX;
	DynamixelMotor* axTest;

public:
	ActuatorsMgr();

	~ActuatorsMgr();

	void testGoto(uint16_t goal) {
		axTest->goalPositionDegree(goal);
	}
};

#endif