// 
// 
// 

#include "ActuatorsMgr.h"

ActuatorsMgr::ActuatorsMgr() : serialAX(DynamixelInterface(Serial1)) {
	serialAX.begin(9600);
	axTest = new DynamixelMotor(serialAX, 0);
	axTest->init();
	axTest->enableTorque();
	axTest->jointMode();
}

ActuatorsMgr::~ActuatorsMgr()
{
}
