#include "Hook.h"

Hook::Hook(int16_t x, int16_t y, uint16_t r, const char* o): zoneX(x), zoneY(y), zoneR(r), order(o)
{
	active = true;
	ready = false;
}

bool Hook::check(int16_t currentX, int16_t currentY)
{
	return (zoneX-currentX)*(zoneX-currentX) + (zoneY - currentY)*(zoneY - currentY) <= zoneR*zoneR;
}

const char* Hook::getOrder()
{
	return order;
}

void Hook::setReady() {
	ready = true;
	
}

bool Hook::isReady() {
	return ready;
}

void Hook::setActive(bool state) {
	active = state;
}