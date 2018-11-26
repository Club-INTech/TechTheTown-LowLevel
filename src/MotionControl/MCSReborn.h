//
// Created by trotfunky on 26/11/18.
//

#ifndef LL_MCSREBORN_H
#define LL_MCSREBORN_H

#include "../Utils/Singleton.hpp"
#include "ControlSettings.h"
#include "RobotStatus.h"

class MCS : public Singleton<MCS>
{

private:
    RobotStatus robotStatus;
    ControlSettings controlSettings;
public:

};

#endif //LL_MCSREBORN_H
