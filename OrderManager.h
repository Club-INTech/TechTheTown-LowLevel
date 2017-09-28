/**
*
*Classe gestionnaire d'ordres reçus depuis le haut niveau
*
*/

#ifndef _ORDERMGR_h
#define _ORDERMGR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif

#include "Singleton.hpp"
#include "MotionControlSystem.h"
#include "SensorMgr.h"
#include "ActuatorsMgr.hpp"
#include "SerialHL.h"
#include "EthernetMgr.h"
#include <WString.h>
#include "defines.h"
#include "Wstring.h"
#include <vector>
#include "utils.h"

class OrderManager : public Singleton<OrderManager>
{
private:
	MotionControlSystem &motionControlSystem;
	SensorMgr &sensorMgr;
	ActuatorsMgr &actuatorsMgr;
	SerialHL &serialHL;

	std::vector<char*> orderData = std::vector<char*>();
	char order[RX_BUFFER_SIZE];

	//Variables booleennes pour envoi de données au HL
	bool isSendingUS;

public:
#if DEBUG
	 SerialHL &highLevel = serialHL;
#else
	 EthernetMgr &highLevel;
#endif

	 OrderManager();
	 void refreshUS();
	 void communicate();
	 void execute(char*);	//public pour pouvoir executer des scripts de hook
	 void sendUSData();
	 int8_t split(char* , std::vector<char*>& , const char* separator = ",");
	 int parseInt(char*);
	 float parseFloat(char*);
};

#endif //_ORDERMGR_h

