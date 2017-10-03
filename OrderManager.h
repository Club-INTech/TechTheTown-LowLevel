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
#include "SerialMgr.h"
#include "EthernetMgr.h"
#include "ActuatorsMgr.h"
#include <WString.h>
#include "defines.h"
#include "Wstring.h"
#include <vector>
#include "utils.h"
#include "Hook.h"

class OrderData {
private:
	std::vector<String> orderData;
public:
	OrderData() : orderData(std::vector<String>()) {}
	void push_back(const String& s) {
		orderData.push_back(s);
	}
	const char* pop() {
		const char* buffer = orderData.at(orderData.size()).c_str();
		orderData.pop_back();
		return buffer;
	}
	const char* at(uint8_t i) {
		return orderData.at(i).c_str();
	}
	void clear() {
		orderData.clear();
	}
	uint8_t size() {
		return orderData.size();
	}
};

class OrderManager : public Singleton<OrderManager>
{
private:
	MotionControlSystem &motionControlSystem;
	SensorMgr &sensorMgr;
	ActuatorsMgr &actuatorsMgr;
	SerialMgr &serialHL;

	OrderData orderData;
	char readMessage[RX_BUFFER_SIZE];
	std::vector<Hook> hookList;

	//Variables booleennes pour envoi de données au HL
	bool isSendingUS;

public:
#if DEBUG
	 SerialMgr &highLevel = serialHL;
#else
	 EthernetMgr &highLevel;
#endif

	 OrderManager();
	 void refreshUS();
	 void communicate();
	 void execute(const char*);	//public pour pouvoir executer des scripts de hook
	 void sendUSData();
	 int8_t split(char* , OrderData& , const char* separator = ",");
	 int parseInt(const char*);
	 float parseFloat(const char*);
	 void hookInterrupt();
	 void executeHooks();
	 bool hooksEnabled;
};



#endif //_ORDERMGR_h

