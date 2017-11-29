// Hook.h

#ifndef _HOOK_h
#define _HOOK_h


#include "WProgram.h"

class Hook
{
private:
	const uint8_t hookID;
	const int16_t zoneX;
	const int16_t zoneY;
	const uint16_t zoneR;
	const float angleTarget;
	const float angleTolerance;

	bool angleTrigger;

	bool active;
	bool ready;
	const char* order;

public:
	Hook(uint8_t, int16_t, int16_t, uint16_t,const char*);
	Hook(uint8_t, int16_t, int16_t, uint16_t, float, float, const char*);
	bool check(int16_t, int16_t);
	bool check(int16_t, int16_t, float);
	const char* getOrder();
	void setActive(bool);
	void setReady();
	bool isReady();
	bool isActive();
	bool isAngleTriggered();
};

#endif

