//
// Created by asphox on 29/04/18.
//

#ifndef TECHTHETOWN_LOWLEVEL_ABSTRACTORDER_H
#define TECHTHETOWN_LOWLEVEL_ABSTRACTORDER_H

#include <Arduino.h>
#include "OrderData.h"
#include "Utils/defines.h"
#include "Actuators/ActuatorValues.h"

typedef OrderData& Args;

class OrderManager;

class AbstractOrder
{
public:
    explicit AbstractOrder(uint8_t nbr_args = 0);
    virtual ~AbstractOrder() = default;
    bool exec(Args args);
    virtual void impl(Args args) = 0;

    inline bool operator()(Args args){ return exec(args); }

protected:
    uint8_t                 nbr_args;
    OrderManager&           orderManager;
};

#define ORDER(name,nbrparam)                         \
struct ORDER_##name : public AbstractOrder           \
{                                                    \
    ORDER_##name() : AbstractOrder(nbrparam)         \
    {}                                               \
    void impl(Args);                                 \
}                                                    \


#endif //TECHTHETOWN_LOWLEVEL_ABSTRACTORDER_H
