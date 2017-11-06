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
#include "MotionControlSystem.h"
#include "pin_mapping.h"

class SensorMgr : public Singleton<SensorMgr>
{
private:
	uint32_t lastRefresh;
	uint32_t delayToRefresh;

public:

	SensorMgr();
	void refresh(MOVING_DIRECTION);

	uint16_t getUsTest();
	bool isJumpEngaged();
	bool isCont1Engaged();
};

#endif

