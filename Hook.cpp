#include "Hook.h"

Hook::Hook(float x, float y, float o, float delta, HookType hookType) :
										motionControlSystem(MotionControlSystem::Instance()),
										actuatorsMgr(ActuatorsMgr::Instance()),
										type(hookType)
{

}