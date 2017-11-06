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

#include <vector>
#include <map>
#include "Singleton.hpp"
#include "MotionControlSystem.h"
#include "SensorMgr.h"
#include "SerialMgr.h"
#include "EthernetMgr.h"
#include "ActuatorsMgr.h"
#include <WString.h>
#include "defines.h"
#include "Wstring.h"
#include "utils.h"
#include "Hook.h"
#include <Metro.h>


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

class HookList 
{
private:
	std::map<uint8_t, Hook> hooks;
	std::vector<uint8_t> ids;
	std::vector<uint8_t> readyIds;

public:
	HookList() : hooks(std::map<uint8_t, Hook>()), ids(std::vector<uint8_t>()), readyIds(std::vector<uint8_t>()){}

	Hook& getHook(uint8_t id) {
		return hooks.at(id);
	}

	void addHook(uint8_t id, int16_t x, int16_t y, uint16_t r, const char* o) {
		hooks.emplace(std::make_pair(id, Hook(id, x, y, r, o)));	//On ajoute un couple (id, hook(id)) au dictionnaire de hooks
		ids.push_back(id);	//On ajoute l'id à la liste des ids disponibles
	}

	void enableHook(uint8_t id) {
		hooks.at(id).setActive(true);
	}

	void disableHook(uint8_t id) {
		hooks.at(id).setActive(false);
	}

	uint8_t getSize() {
		return ids.size();
	}

	uint8_t getReadySize() {
		return readyIds.size();
	}

	const char* getReadyHookOrder(uint8_t i) {
		hooks.at(readyIds.at(i)).setActive(false); //Si un hook est à effectuer maintenant, il ne sera plus activable
		return hooks.at(readyIds.at(i)).getOrder();
	}
	void check(float x, float y) {
		for (uint8_t i = 0; i < ids.size(); ++i) {
			Hook currentHook = (hooks.at(ids.at(i)));
			if (currentHook.isActive() && !currentHook.isReady() && currentHook.check(x, y)) {
				currentHook.setReady();			//Les conditions du hook sont réunies !
				readyIds.push_back(ids.at(i));	//Il faudra l'executer dès que possible
			}
		}
	}
};


class OrderManager : public Singleton<OrderManager>
{
private:
	MotionControlSystem &motionControlSystem;
	SensorMgr &sensorMgr;
	ActuatorsMgr &actuatorsMgr;
	SerialMgr &serialHL;
	HookList hookList;
	OrderData orderData;
	char readMessage[RX_BUFFER_SIZE];

	//Variables booleennes pour envoi de données au HL
	bool isSendingUS;

public:
#if DEBUG
	 SerialMgr &highLevel = serialHL;
#else
	 EthernetMgr &highLevel;
#endif

	 OrderManager();
	 
	 //Com&exec
	 void refreshUS();
	 void communicate();
	 void execute(const char*);	//public pour pouvoir executer des scripts de hook
	 void sendUSData();

	 //Utilitaire
	 int8_t split(char* , OrderData& , const char* separator = ",");
	 int parseInt(const char*);
	 float parseFloat(const char*);

	 //Hooks
	 void checkHooks();
	 void executeHooks();
	 bool hooksEnabled;
};



#endif //_ORDERMGR_h

