//
// Created by trotfunky on 18/12/17.
//

#ifndef LL_DYNAMIXELMGR_H
#define LL_DYNAMIXELMGR_H

#include "Arduino.h"

#include <map>
#include <vector>

#include "Utils/Singleton.hpp"

#include "DynamixelGroup.h"

#include "Dynamixel.h"
#include "DynamixelInterface.h"
#include "DynamixelMotor.h"

/**
 * Classe de gestion des AX12
 * Les garde en mémoire et permet de les contrôler
 */

class DynamixelMgr : public Singleton<DynamixelMgr>
{

    public:

    DynamixelMgr();
    ~DynamixelMgr();

    //Controlling

    bool movAX12(int, uint16_t);
    bool setAX12Speed(int, uint16_t);

    bool movAX12G(unsigned int, uint16_t);
    bool setAX12GSpeed(unsigned int, uint16_t);



    private:

    DynamixelInterface serialAX;
    std::map<int,DynamixelMotor*> axList;
    std::vector<DynamixelGroup> axGroupsList;

    //Managing

    bool addAX12(int);
    bool addAX12(int, uint16_t);
    bool checkIfAX(int);

    void addAX12Group();
    bool populateAX12Group(unsigned int, int, DynSym);
    bool populateAX12Group(unsigned int, int, uint16_t, DynSym);

};


#endif //LL_DYNAMIXELMGR_H
