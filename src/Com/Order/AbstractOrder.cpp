//
// Created by asphox on 30/04/18.
//

#include "Com/Order/AbstractOrder.h"
#include "OrderManager.h"

AbstractOrder::AbstractOrder(uint8_t nbr_args)
                :nbr_args(nbr_args)
                ,orderManager(OrderManager::Instance())
{}

bool AbstractOrder::exec(Args args)
{
    if(args.nbrParams() >= nbr_args)
    {
        impl(args);
        return true;
    }
    else
    {
        orderManager.highLevel.printfln(DEBUG_HEADER,"ERREUR: Nombre de paramètres incorrects");
        return false;
    }
}
