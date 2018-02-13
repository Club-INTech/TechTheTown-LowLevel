// SensorMgr.h

#ifndef _SENSORMGR_h
#define _SENSORMGR_h

#include <Arduino.h>
#include <vector>
#include <algorithm>
#include <i2c_t3.h>

#include "Utils/Singleton.hpp"
#include "Utils/pin_mapping.h"
#include "MotionControlSystem.h"
#include "Utils/pin_mapping.h"
#include "Com/EthernetMgr.h"
#include "Com/SerialMgr.h"
#include "SRF10.h"
#include "AbstractSensorUS.h"


class SensorMgr : public Singleton<SensorMgr>
{


private:
	#if DEBUG
	 SerialMgr &highLevel = SerialMgr::Instance();
	#else
	 EthernetMgr &highLevel = EthernetMgr::Instance();
	#endif

	AbstractSensorUS* US[NBR_OF_US_CAPTOR];
	std::vector<uint16_t> distances;
	uint8_t currentMeasuringUS = 0;
	bool isMeasuring = false;
	bool firstMeasure = true;
	MOVING_DIRECTION measure_direction;

public:

	SensorMgr();
	void refresh(MOVING_DIRECTION dir);

	bool isJumpEngaged();
	bool isCont1Engaged();
};

#endif
