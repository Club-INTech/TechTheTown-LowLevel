// ActuatorsMgr.h


#ifndef _ACTUATORSMGR_h
#define _ACTUATORSMGR_h

#include "Arduino.h"

#include "DynamixelMgr.h"

#include "Utils/pin_mapping.h"
#include "Utils/Singleton.hpp"

class ActuatorsMgr : public Singleton<ActuatorsMgr>
{
private:


public:

	ActuatorsMgr();
	~ActuatorsMgr();


	//Gestion des AX12
    DynamixelMgr& dynamixelMgr;

	//Contrôle des AX12
	void movAX12(int, uint16_t);
	void setAX12Speed(int, uint16_t);

	void movAX12G(unsigned int, uint16_t);
	void setAX12GSpeed(unsigned int, uint16_t);

	//Contrôle de la pompe

    bool isPumpOn;
	bool togglePumpState(bool);

    //Contrôle des électrovannes
    bool isElecVanneAVOpen;
    bool isElecVanneAROpen;
    void setElecVanneAV(bool);
    void setElecVanneAR(bool);

};

#endif