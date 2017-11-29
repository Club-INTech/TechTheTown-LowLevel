// Hook.h

#ifndef _HOOK_h
#define _HOOK_h


#include "WProgram.h"

class Hook
{
private:
	const uint8_t hookID;
	const int32_t zoneX;
	const int32_t zoneY;
	const uint32_t zoneR;
	const float angleTarget;
	const float angleTolerance;

	bool active;
	bool ready;
	const char* order;

public:
	Hook(uint8_t, int32_t, int32_t, uint32_t, float, float, const char*);
	bool check(int32_t, int32_t, float);
	const char* getOrder();
	void setActive(bool);
	void setReady();
	bool isReady();
	bool isActive();
};

#endif

