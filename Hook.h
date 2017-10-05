// Hook.h

#ifndef _HOOK_h
#define _HOOK_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "wprogram.h"
#else
	#include "WProgram.h"
#endif

class Hook
{
private:
	const uint8_t hookID;
	const int16_t zoneX;
	const int16_t zoneY;
	const uint16_t zoneR;
	bool active;
	bool ready;
	const char* order;

public:
	Hook(uint8_t, int16_t, int16_t, uint16_t,const char*);
	bool check(int16_t, int16_t);
	const char* getOrder();
	void setActive(bool);
	void setReady();
	bool isReady();
};

#endif

