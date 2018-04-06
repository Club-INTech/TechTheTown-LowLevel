// SensorMgr.h

#ifndef _SENSORMGR_h
#define _SENSORMGR_h

#include <Arduino.h>
#include <vector>
#include <array>
#include <algorithm>
#include <i2c_t3.h>

#include "Utils/average.hpp"
#include "Utils/Singleton.hpp"
#include "Utils/pin_mapping.h"
#include "MotionControl/MotionControlSystem.h"
#include "Utils/pin_mapping.h"
#include "Com/EthernetMgr.h"
#include "Com/SerialMgr.h"
#include "SRF10.h"
#include "VL6180X.h"
#include "AbstractSensorUS.h"


class SensorMgr : public Singleton<SensorMgr>
{


private:
	#if DEBUG
	 SerialMgr &highLevel = SerialMgr::Instance();
	#else
	 EthernetMgr &highLevel = EthernetMgr::Instance();
	#endif


	SRF10* US[NBR_OF_US_SENSOR];
	VL6180X sensorCubeAV;
	VL6180X sensorCubeAR;

	std::vector<Average<uint16_t,AVERAGE_US_SIZE>> distances;
	uint8_t currentMeasuringUS = 0;
	bool isMeasuring = false;
	bool firstMeasure = true;
	bool sendRequest= false;
	MOVING_DIRECTION measure_direction;

	bool jumperPlugged;
    bool isBasicDetectionOn;
	unsigned int basicBlocked;
public:

	SensorMgr();
	void refreshUS(MOVING_DIRECTION dir);
	void checkCubeAV();
	void checkCubeAR();
	void sendUS();

	bool isJumperEngaged();
	bool isReadyToGo();
	bool isCont1Engaged();

    void enableBasicDetection(bool);
	bool isBasicBlocked();
	void resetBasicBlocked();
};

#endif
