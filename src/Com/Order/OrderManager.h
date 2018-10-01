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
#include "Com/Hooks/Hook.h"
#include "Actuators/DynamixelGroup.h"
#include "OrderData.h"
#include "Orders.h"
#include "Com/Hooks/HookList.h"

class OrderManager : public Singleton<OrderManager>
{
private:

	std::map<String, AbstractOrder*> orders;

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
