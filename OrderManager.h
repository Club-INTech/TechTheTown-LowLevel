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
class OrderManager : public Singleton<OrderManager>
{
private:
	MotionControlSystem &motionControlSystem;
	SensorMgr &sensorMgr;
	ActuatorsMgr &actuatorsMgr;
	SerialHL &serialHL;
	EthernetMgr &ethMgr;
#if DEBUG
	SerialHL &highLevel= serialHL;
#else
	EthernetMgr &highLevel = ethMgr;
#endif
	String order;

	//Variables booleennes pour envoi de données au HL
	bool isSendingUS;

 public:
	 OrderManager();

	 void refreshUS();
	 void receiveAndExecute();
	 void sendUSData();
	 void ack();
};

#endif //_ORDERMGR_h

