#include "SensorMgr.h"

SensorMgr::SensorMgr():
	usTEST(PIN_TRIGGER_TEST, PIN_ECHO_TEST)
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
/*
void SensorMgr::interruptAVG()
{
	usAVG.interruption();
}

void SensorMgr::interruptAVD()
{
	usAVG.interruption();
}

void SensorMgr::interruptARG()
{
	usAVG.interruption();
}

void SensorMgr::interruptARD()
{
	usAVG.interruption();
}

uint16_t SensorMgr::getUsAVG()
{
	return usAVG.value();
}

uint16_t SensorMgr::getUsAVD()
{
	return usAVD.value();
}

uint16_t SensorMgr::getUsARG()
{
	return usARG.value();
}

uint16_t SensorMgr::getUsARD()
{
	return usARD.value();
}
*/
void SensorMgr::interruptTest() {
	usTEST.interruption();
}
uint16_t SensorMgr::getUsTest()
{
	return usTEST.value();
}

bool SensorMgr::isJumpEngaged()
{
	return digitalRead(PIN_JMPR);
}

bool SensorMgr::isContEngaged()
{
	return digitalRead(PIN_CONT);
}