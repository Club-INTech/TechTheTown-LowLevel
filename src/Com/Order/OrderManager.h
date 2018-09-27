/**
*Contient la liste des correspondances pour les entrées série du programme
*
* @author caillou, sylvain, rémi, melanie, Ug
*
**/

#ifndef _ORDERMGR_h
#define _ORDERMGR_h

#include "Metro.h"
#include <map>
#include <set>
#include <string>
#include <cstdlib>
#include <Arduino.h>
#include <WString.h>
#include "Utils/Singleton.hpp"
#include "MotionControl/MotionControlSystem.h"
#include "Sensors/SensorMgr.h"
#include "Com/ComMgr.h"
#include "Actuators/ActuatorsMgr.h"
#include "Utils/defines.h"
#include "Utils/utils.h"
#include "Com/Hook.h"
#include "Actuators/DynamixelGroup.h"
#include "OrderData.h"
#include "Orders.h"

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

    void addHook(uint8_t id, int32_t x, uint32_t y, uint32_t r, float alpha, float tolerance, const char* o) {
        hooks.insert(std::make_pair(id, Hook(id, x, y, r, alpha, tolerance, o)));  //On initialise le hook
    }

	void enableHook(uint8_t id) {
		hooks.at(id).setActive(true);
	}

	void disableHook(uint8_t id) {
		hooks.at(id).setActive(false);
	}

	const String getReadyHookOrder(uint8_t i) {
		hooks.at(i).setActive(false);               //Si un hook est à effectuer maintenant, il ne sera plus activable
		return hooks.at(i).getOrder();
	}
	void check(int32_t x, uint32_t y, float alpha) {
		auto start = hooks.begin();
		auto end = hooks.end();
		while (start != end )
		{
			Hook &currentHook = start->second;
            if (currentHook.isActive() && !currentHook.isReady() && currentHook.check(x, y,alpha))
            {
                currentHook.setReady();             //Les conditions du hook sont réunies !
                readyIds.push_back(start->first);	//Il faudra l'executer dès que possible
            }
			++start;
        }
	}

    std::vector<String> executeHooks()
    {
        std::vector<String> orders;

        auto start = readyIds.begin();
        auto end = readyIds.end();
        while ( start != end )
        {
            orders.push_back(getReadyHookOrder(*start));
            hooks.at(*start).setActive(true);
            ++start;
        }
        readyIds.clear();
        return(orders);
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

	std::map<const char*, AbstractOrder*> orders;

	bool basicDetectionTriggeredSent;
	bool basicDetectionFinishedSent;
	char readMessage[RX_BUFFER_SIZE];
    char charIDLastMessage;

public:
	HookList hookList;
	OrderData orderData;
	SensorMgr &sensorMgr;
	MotionControlSystem& motionControlSystem;
	ActuatorsMgr &actuatorsMgr;
	ComMgr& highLevel;

    //Variables booleennes pour envoi de données au HL
    bool isSendingUS;
	bool HLWaiting;

    OrderManager();

    void init();

    //Com&exec
    inline void refreshUS(){
		if(isSendingUS)
			sensorMgr.refreshUS(motionControlSystem.getMovingDirection());
	}
	inline void sendUS(){
		sensorMgr.sendUS();
	}
    void communicate();
    void execute(const char*);	//public pour pouvoir executer des scripts de hook


    //Utilitaire
    int8_t split(char* , OrderData& , const char* separator = " ");
    int parseInt(const char*);
    float parseFloat(const char*);
	bool isHLWaiting();
	void checkJumper();

    //Hooks
    void checkHooks();
    void executeHooks();
    bool hooksEnabled;


//    std::map<std::string,int> lookupTable;
};

#endif //_ORDERMGR_h
