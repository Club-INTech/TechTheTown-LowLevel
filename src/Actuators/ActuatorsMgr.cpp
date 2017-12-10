// 
// 
// 

#include "ActuatorsMgr.h"

ActuatorsMgr::ActuatorsMgr() : serialAX(DynamixelInterface(Serial1))
{
	serialAX.begin(9600);
}

ActuatorsMgr::~ActuatorsMgr()
{
}

bool ActuatorsMgr::addAX12(int id)
{
	if(checkIfAX(id))
	{
		return(false);
	}
	axList.insert({id,new DynamixelMotor(serialAX,id)});
	axList.at(id)->init();
	axList.at(id)->enableTorque();
	axList.at(id)->jointMode();
	return(true);
}

bool ActuatorsMgr::addAX12(int id, uint16_t speed)	//Initialise l'AX avec une vitesse définie
{
	if(!this->addAX12(id))
	{
		return(false);
	}
	axList.at(id)->speed(speed);
	return(true);
}

bool ActuatorsMgr::checkIfAX(int id)
{
	return(axList.count(id)!=0);
}


bool ActuatorsMgr::movAX12(int id, uint16_t angleDegree)
{
	if(!checkIfAX(id))
	{
		return(false);
	}
	axList.at(id)->goalPositionDegree(angleDegree);
	return(true);
}
bool ActuatorsMgr::setAX12Speed(int id, uint16_t speed)
{
	if(!checkIfAX(id))
	{
		return(false);
	}
	axList.at(id)->speed(speed);
	return(true);
}

void ActuatorsMgr::addAX12Group()
{
	axGroupsList.emplace_back();
}

bool ActuatorsMgr::populateAX12Group(int groupId, int motorId, DynSym symmetry)
{
	if(!addAX12(motorId))
	{
		return(false);
	}
	axGroupsList.at(groupId).add(axList.at(motorId),symmetry);
	return(true);
}

bool ActuatorsMgr::populateAX12Group(int groupId, int motorId, uint16_t speed, DynSym symmetry)
{
	if(!this->populateAX12Group(groupId, motorId, symmetry))
	{
		return(false);
	}
	axGroupsList.at(groupId).speed(speed);
	return(true);

}

bool ActuatorsMgr::movAX12G(unsigned int groupId, uint16_t angleDegree)
{
	if(groupId>=axGroupsList.size())
	{
		return(false);
	}
	axGroupsList.at(groupId).goalPositionDegree(angleDegree);
	return(true);
}

bool ActuatorsMgr::setAX12GSpeed(unsigned int groupId, uint16_t speed)
{
	if(groupId>=axGroupsList.size())
	{
		return(false);
	}
	axGroupsList.at(groupId).speed(speed);
	return(true);
}