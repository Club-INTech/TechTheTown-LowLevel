#include "Hook.h"

Hook::Hook(int16_t x, int16_t y, uint16_t r, char* o): zoneX(x), zoneY(y), zoneR(r), order(o)
{
	active = true;
}

bool Hook::check(int16_t currentX, int16_t currentY)
{
	return (zoneX-currentX)*(zoneX-currentX) + (zoneY - currentY)*(zoneY - currentY) <= zoneR*zoneR;
}

void Hook::execute(OrderManager& orderMgr)
{
	orderMgr.execute(order);
}
