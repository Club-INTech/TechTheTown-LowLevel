//
// Created by trotfunky on 18/12/17.
//

#include "DynamixelMgr.h"

DynamixelMgr::DynamixelMgr() : serialAX(DynamixelInterface(Serial1))
{
    serialAX.begin(9600);

    //Ajouter les actionneurs ici

    addAX12Group();
    populateAX12Group(0,1,120,BASE);
    populateAX12Group(0,2,120,MIRROR);  //Bras et porte avant
    addAX12(3,200);
    addAX12Group();
    populateAX12Group(1,4,120,BASE);
    populateAX12Group(1,5,120,MIRROR);  //Bras et porte arrière
    addAX12(6,200);
}

DynamixelMgr::~DynamixelMgr()
{
}

bool DynamixelMgr::addAX12(int id)
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
bool DynamixelMgr::addAX12(int id, uint16_t speed)	//Initialise l'AX avec une vitesse définie
{
    if(!this->addAX12(id))
    {
        return(false);
    }
    axList.at(id)->speed(speed);
    return(true);
}

void DynamixelMgr::addAX12Group()
{
    DynamixelGroup tempo;
    axGroupsList.push_back(tempo);
}

bool DynamixelMgr::populateAX12Group(int groupId, int motorId, DynSym symmetry)
{
    if(!addAX12(motorId))
    {
        return(false);
    }
    axGroupsList.at(groupId).add(axList.at(motorId),symmetry);
    return(true);
}

bool DynamixelMgr::populateAX12Group(int groupId, int motorId, uint16_t speed, DynSym symmetry)
{
    if(!this->populateAX12Group(groupId, motorId, symmetry))
    {
        return(false);
    }
    axGroupsList.at(groupId).speed(speed);
    return(true);

}

bool DynamixelMgr::movAX12(int id, uint16_t angleDegree)
{
    if(!checkIfAX(id))
    {
        return(false);
    }
    axList.at(id)->goalPositionDegree(angleDegree);
    return(true);
}
bool DynamixelMgr::setAX12Speed(int id, uint16_t speed)
{
    if(!checkIfAX(id))
    {
        return(false);
    }
    axList.at(id)->speed(speed);
    return(true);
}

bool DynamixelMgr::movAX12G(unsigned int groupId, uint16_t angleDegree)
{
    if(groupId>=axGroupsList.size())
    {
        return(false);
    }
    axGroupsList.at(groupId).goalPositionDegree(angleDegree);
    return(true);
}

bool DynamixelMgr::setAX12GSpeed(unsigned int groupId, uint16_t speed)
{
    if(groupId>=axGroupsList.size())
    {
        return(false);
    }
    axGroupsList.at(groupId).speed(speed);
    return(true);
}


bool DynamixelMgr::checkIfAX(int id)
{
    return(axList.count(id)!=0);
}
