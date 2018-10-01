// Hook.h

#ifndef _HOOK_h
#define _HOOK_h

#include "Com/Interfaces/EthernetInterface.h"
#include "WProgram.h"

class Hook
{
private:
	const uint8_t hookID;
	const int32_t zoneX;
	const uint32_t zoneY; //Y jamais < 0
	const uint32_t zoneR;
	const float angleTarget;
	const float angleTolerance;

	bool active;
	bool ready;
	const String order;

public:
	Hook(uint8_t, int32_t, uint32_t, uint32_t, float, float, const char*);
	bool check(uint32_t, uint32_t, float);

	const String getOrder();
	void setActive(bool);
	void setReady();
	bool isReady();
	bool isActive();
};

#endif

