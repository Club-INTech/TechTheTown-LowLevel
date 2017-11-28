#include <Utils/utils.h>
#include "Hook.h"

Hook::Hook(uint8_t id, int16_t x, int16_t y, uint16_t r, const char* o) : hookID(id), zoneX(x), zoneY(y), zoneR(r), order(o), angleTarget(0),angleTolerance(0)
{
	active = true;
	ready = false;

	angleTrigger = false;
}

Hook::Hook(uint8_t id, int16_t x, int16_t y, uint16_t r, float alpha, float tolerance, const char* o) : hookID(id), zoneX(x), zoneY(y), zoneR(r),angleTarget(alpha),angleTolerance(tolerance), order(o)
{
	active = true;
	ready = false;

	angleTrigger = true;
}

bool Hook::check(int16_t currentX, int16_t currentY)
{
	return (zoneX-currentX)*(zoneX-currentX) + (zoneY - currentY)*(zoneY - currentY) <= zoneR*zoneR;
}

bool Hook::check(int16_t currentX, int16_t currentY, float currentAngle)
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