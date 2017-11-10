// SensorMgr.h

#ifndef _SENSORMGR_h
#define _SENSORMGR_h

#include <Arduino.h>

#include "Utils/Singleton.hpp"
#include "Utils/pin_mapping.h"
#include "MotionControl/MotionControlSystem.h"
#include "Utils/pin_mapping.h"

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

