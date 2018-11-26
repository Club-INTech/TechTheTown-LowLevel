//
// Created by trotfunky on 26/11/18.
//

#ifndef LL_MCSREBORN_H
#define LL_MCSREBORN_H

#include "../Utils/Singleton.hpp"
#include "ControlSettings.h"

class MCS : public Singleton<MCS>
{
private:

    ControlSettings controlSettings;
public:

};

#endif //LL_MCSREBORN_H
