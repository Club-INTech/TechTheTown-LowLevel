#include <Utils/utils.h>
#include <Utils/defines.h>
#include "Hook.h"


Hook::Hook(uint8_t id, int32_t x, uint32_t y, uint32_t r, float alpha, float tolerance, const char* o) : hookID(id), zoneX(x), zoneY(y), zoneR(r),angleTarget(alpha),angleTolerance(tolerance), order(o)
{
	active = false;
	ready = false;
}



bool Hook::check(uint32_t currentX, uint32_t currentY, float currentAngle)
{
	if(ABS(currentAngle-angleTarget) > PI)
	{
		if(angleTarget<0)
		{
			currentAngle -= TWO_PI;
		}
		else
		{
			currentAngle += TWO_PI;
		}
	}
#if DEBUG

	static EthernetMgr& highLevel = EthernetMgr::Instance();

	highLevel.println("On fait un check des hooks avec les positions suivantes");
	highLevel.print("Position cibleX: ");
	highLevel.println(zoneX);
	highLevel.print("Position X: ");
	highLevel.println(currentX);
	highLevel.print("Position cibleY: ");
	highLevel.println(zoneY);
	highLevel.print("Position Y: ");
	highLevel.println(currentY);
	highLevel.print("Angle actuel: ");
	highLevel.println(currentAngle);
	highLevel.print("Angle cible: ");
	highLevel.println(angleTarget);
	highLevel.print("Delta: ");
	highLevel.println(ABS(currentAngle-angleTarget));
	highLevel.print("Tolérance: ");
	highLevel.println(angleTolerance);
    highLevel.print("Ordre à executer: ");
    highLevel.println(order);
	highLevel.print("Hook pos state:");
	highLevel.println((zoneX-currentX)*(zoneX-currentX) + (zoneY - currentY)*(zoneY - currentY) <= zoneR*zoneR);
	highLevel.print("Hook angle state:");
    highLevel.println(ABS((currentAngle+PI)-(angleTarget+PI)) <= angleTolerance);
	highLevel.println("=====");
#endif

	return ((zoneX-currentX)*(zoneX-currentX) + (zoneY - currentY)*(zoneY - currentY) <= zoneR*zoneR
			&& (ABS(currentAngle-angleTarget) <= angleTolerance));
}

const String Hook::getOrder()
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