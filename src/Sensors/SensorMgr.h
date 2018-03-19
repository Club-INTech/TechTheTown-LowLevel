// SensorMgr.h

#ifndef _SENSORMGR_h
#define _SENSORMGR_h

#include <Arduino.h>
#include <vector>
#include <array>
#include <algorithm>
#include <i2c_t3.h>

#include "Utils/Singleton.hpp"
#include "Utils/pin_mapping.h"
#include "MotionControl/MotionControlSystem.h"
#include "Utils/pin_mapping.h"
#include "Com/EthernetMgr.h"
#include "Com/SerialMgr.h"
#include "SRF10.h"
#include "PassageCounter.h"
#include "AbstractSensorUS.h"


class SensorMgr : public Singleton<SensorMgr>
{


private:
	#if DEBUG
	 SerialMgr &highLevel = SerialMgr::Instance();
	#else
	 EthernetMgr &highLevel = EthernetMgr::Instance();
	#endif

	IntervalTimer m_timer_update_PC_cube_AV;
	IntervalTimer m_timer_update_PC_cube_AR;


	SRF10* US[NBR_OF_US_SENSOR];
	PassageCounter* PC_cube_av;
	//PassageCounter* PC_cube_ar;


	std::vector<uint16_t> distances;
	uint8_t currentMeasuringUS = 0;
	bool isMeasuring = false;
	bool firstMeasure = true;
	MOVING_DIRECTION measure_direction;

public:

	SensorMgr();
	void refreshUS(MOVING_DIRECTION dir);
	//refresh PassageCounters
	void refreshCPAV();
	void refreshCPAR();
	void enableCheckCPAV();
	void enableCheckCPAR();
	void disableCheckCP();

	bool isJumperEngaged();
	bool isCont1Engaged();
};

#endif
