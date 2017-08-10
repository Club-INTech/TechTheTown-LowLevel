// SensorMgr.h

#ifndef _SENSORMGR_h
#define _SENSORMGR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif

#include "Singleton.hpp"
#include "pin_mapping.h"
#include "us_sr04.h"
#include "MotionControlSystem.h"
#include "pin_mapping.h"

class SensorMgr : public Singleton<SensorMgr>
{
private:
	SR04 usTEST;
	uint32_t lastRefresh;
	uint32_t delayToRefresh;

public:

	SensorMgr();
	void refresh(MOVING_DIRECTION);

	/*void interruptAVG();
	void interruptAVD();
	void interruptARG();
	void interruptARD();

	uint16_t getUsAVG();
	uint16_t getUsAVD();
	uint16_t getUsARG();
	uint16_t getUsARD();
	*/
	void interruptTest();
	uint16_t getUsTest();
	bool isJumpEngaged();
	bool isContEngaged();
};

#endif

