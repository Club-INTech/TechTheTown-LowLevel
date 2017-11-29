/**
*Contient la liste des correspondances pour les entrées série du programme
*
* @author caillou, sylvain, rémi, melanie, Ug
*
**/

#ifndef _ORDERMGR_h
#define _ORDERMGR_h

#include <Arduino.h>


#include <vector>
#include <map>
#include "Utils/Singleton.hpp"
#include "MotionControl/MotionControlSystem.h"
#include "MotionControl/SensorMgr.h"
#include "SerialMgr.h"
#include "Com/EthernetMgr.h"
#include "Actuators/ActuatorsMgr.h"
#include "Utils/defines.h"
#include "WString.h"
#include "Utils/utils.h"
#include "Com/Hook.h"
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
	std::vector<uint8_t> readyIds;

public:
	HookList() : hooks(std::map<uint8_t, Hook>()), readyIds(std::vector<uint8_t>()){}

	Hook& getHook(uint8_t id) {
		return hooks.at(id);
	}

	void addHook(uint8_t id, int16_t x, int16_t y, uint16_t r, const char* o) {
		hooks.emplace(std::make_pair(id, Hook(id, x, y, r, o)));	//On ajoute un couple (id, hook(id)) au dictionnaire de hooks
	}
    void addHook(uint8_t id, int16_t x, int16_t y, uint16_t r, float alpha, float tolerance, const char* o) {
        hooks.emplace(std::make_pair(id, Hook(id, x, y, r, alpha, tolerance, o)));  //Idem mais avec un angle
    }

	void enableHook(uint8_t id) {
		hooks.at(id).setActive(true);
	}

	void disableHook(uint8_t id) {
		hooks.at(id).setActive(false);
	}

	int getSize() {
		return hooks.size();
	}

	int getReadySize() {
		return readyIds.size();
	}

	const char* getReadyHookOrder(uint8_t i) {
		hooks.at(readyIds.at(i)).setActive(false); //Si un hook est � effectuer maintenant, il ne sera plus activable
		return hooks.at(readyIds.at(i)).getOrder();
	}
	void check(float x, float y, float alpha) {
		auto start = hooks.begin();
		auto end = hooks.end();
		while (start != end )
		{
			Hook currentHook = start->second;
            if(currentHook.isAngleTriggered())
            {
                if (currentHook.isActive() && !currentHook.isReady() && currentHook.check(x, y,alpha))
                {
                    currentHook.setReady();			//Les conditions du hook sont r�unies !
                    readyIds.push_back(start->first);	//Il faudra l'executer d�s que possible
                }
            }
            else
            {
                if (currentHook.isActive() && !currentHook.isReady() && currentHook.check(x, y))
                {
                    currentHook.setReady();			//Les conditions du hook sont r�unies !
                    readyIds.push_back(start->first);	//Il faudra l'executer d�s que possible
                }
            }
		}
	}

    bool hookWithId(int hookId)
    {
        auto start = hooks.begin();
        auto end = hooks.end();
        while (start != end )
        {
            if(start->first == hookId)
            {
                return true;
            }
            ++start;
        }
        return false;
    }
};


class OrderManager : public Singleton<OrderManager>
{
private:
	MotionControlSystem& motionControlSystem;
	SensorMgr &sensorMgr;
	ActuatorsMgr &actuatorsMgr;
	HookList hookList;
	OrderData orderData;
	char readMessage[RX_BUFFER_SIZE];

	//Variables booleennes pour envoi de donn�es au HL
	bool isSendingUS;

public:
	#if DEBUG
	 SerialMgr &highLevel;
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
	 uint8_t split(char* , OrderData& , const char* separator = " ");
	 int parseInt(const char*);
	 float parseFloat(const char*);

	 //Hooks
	 void checkHooks();
	 void executeHooks();
	 bool hooksEnabled;
};

#endif //_ORDERMGR_h

