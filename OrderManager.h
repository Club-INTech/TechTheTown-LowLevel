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
#include <vector>
#include "utils.h"

class OrderManager : public Singleton<OrderManager>
{
private:
	MotionControlSystem &motionControlSystem;
	SensorMgr &sensorMgr;
	ActuatorsMgr &actuatorsMgr;
	SerialHL &serialHL;
#if DEBUG
	SerialHL &highLevel=serialHL;
#else
	EthernetMgr &highLevel;
#endif
	char order[RX_BUFFER_SIZE];
	char orderData[RX_WORD_COUNT][RX_WORD_SIZE];
	//Variables booleennes pour envoi de données au HL
	bool isSendingUS;

 public:
	 OrderManager();

	 void refreshUS();
	 void receiveAndExecute();
	 void sendUSData();
	 uint8_t split(char input[RX_BUFFER_SIZE], char output[4][RX_WORD_SIZE], const char* separator = " ");
};

#endif //_ORDERMGR_h

