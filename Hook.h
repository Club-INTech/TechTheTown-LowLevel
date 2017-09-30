// Hook.h

#ifndef _HOOK_h
#define _HOOK_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "wprogram.h"
#else
	#include "WProgram.h"
#endif

#include "OrderManager.h"

class Hook
{
private:
	int16_t zoneX;
	int16_t zoneY;
	uint16_t zoneR;
	bool active;
	char* order;

public:
	Hook(int16_t, int16_t, uint16_t, char*);
	bool check(int16_t, int16_t);
	void execute(OrderManager&);
};

#endif

