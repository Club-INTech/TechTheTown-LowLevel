// ActuatorsMgr.h


#ifndef _ACTUATORSMGR_h
#define _ACTUATORSMGR_h

#include "Arduino.h"

#include <map>
#include <vector>
#include <Utils/pin_mapping.h>

#include "Dynamixel.h"
#include "DynamixelInterface.h"
#include "DynamixelMotor.h"
#include "Utils/Singleton.hpp"
#include "DynamixelGroup.h"

class ActuatorsMgr : public Singleton<ActuatorsMgr>
{
private:

	DynamixelInterface serialAX;

	std::map<int,DynamixelMotor*> axList;

	std::vector<DynamixelGroup> axGroupsList;

	bool pumpCurrentState;

public:

	ActuatorsMgr();
	~ActuatorsMgr();


	//Gestion des AX12
	bool addAX12(int);
	bool addAX12(int, uint16_t);
	bool checkIfAX(int);

	void addAX12Group();
	bool populateAX12Group(int, int, DynSym);
	bool populateAX12Group(int, int, uint16_t, DynSym);

	//Contrôle des AX12
	bool movAX12(int, uint16_t);
	bool setAX12Speed(int, uint16_t);

	bool movAX12G(unsigned int, uint16_t);
	bool setAX12GSpeed(unsigned int, uint16_t);

	//Contrôle de la pompe
	bool setPumpState(bool);
	uint8_t pin_pwm_pompe=PIN_PWM_POMPE;

};

#endif