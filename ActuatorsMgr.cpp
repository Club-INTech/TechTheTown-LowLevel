// 
// 
// 

#include "ActuatorsMgr.h"

ActuatorsMgr::ActuatorsMgr() : serialAX(DynamixelInterface(Serial1)) ,
							   axTest(DynamixelMotor(serialAX, 0))
{
	serialAX.begin(9600);

	axTest.init();
	axTest.enableTorque();
	axTest.jointMode();
}

ActuatorsMgr::~ActuatorsMgr()
{
}

void ActuatorsMgr::testGoto(uint16_t goal) {
	axTest.goalPositionDegree(goal);
}