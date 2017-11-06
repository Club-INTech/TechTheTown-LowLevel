#include "SensorMgr.h"

SensorMgr::SensorMgr()
{
	delayToRefresh = 25;
	lastRefresh = millis();
}

void SensorMgr::refresh(MOVING_DIRECTION dir)
{
	if (millis() - lastRefresh > delayToRefresh) {
		/*if (dir == MOVING_DIRECTION::FORWARD) {
			usAVG.refresh();
			usAVD.refresh();
		}
		else if (dir == MOVING_DIRECTION::BACKWARD) {
			usARG.refresh();
			usARD.refresh();
		}
		else if (dir == MOVING_DIRECTION::NONE) {
			usAVG.refresh();
			usAVD.refresh();
			usARG.refresh();
			usARD.refresh();
		}*/
		lastRefresh = millis();
	}
}

uint16_t SensorMgr::getUsTest()
{
	//return usTEST.value();
	return 0;
}

//Contacteurs et Jumper

bool SensorMgr::isJumpEngaged()
{
	return digitalRead(PIN_JMPR);
}

bool SensorMgr::isCont1Engaged()
{
	return digitalRead(PIN_CONT1);
}