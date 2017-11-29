#include <Utils/utils.h>
#include "Hook.h"


Hook::Hook(uint8_t id, int32_t x, int32_t y, uint32_t r, float alpha, float tolerance, const char* o) : hookID(id), zoneX(x), zoneY(y), zoneR(r),angleTarget(alpha),angleTolerance(tolerance), order(o)
{
	active = true;
	ready = false;
}



bool Hook::check(int32_t currentX, int32_t currentY, float currentAngle)
{
	return ((zoneX-currentX)*(zoneX-currentX) + (zoneY - currentY)*(zoneY - currentY) <= zoneR*zoneR
			&& (ABS(currentAngle-angleTarget) <= angleTolerance));
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

bool Hook::isActive() {
	return active;
}