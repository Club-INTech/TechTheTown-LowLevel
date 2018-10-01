//
// Created by asphox on 27/09/18.
//

#ifndef TECHTHETOWN_LOWLEVEL_HOOKLIST_H
#define TECHTHETOWN_LOWLEVEL_HOOKLIST_H

#include <map>
#include <vector>
#include "Hook.h"

class HookList
{
private:
    std::map<uint8_t, Hook> hooks;
    std::vector<uint8_t> readyIds;

public:
    HookList() : hooks(std::map<uint8_t, Hook>()), readyIds(std::vector<uint8_t>()){}

    Hook& getHook(uint8_t id) {
        return hooks.at(id);
    }

    void addHook(uint8_t id, int32_t x, uint32_t y, uint32_t r, float alpha, float tolerance, const char* o) {
        hooks.insert(std::make_pair(id, Hook(id, x, y, r, alpha, tolerance, o)));  //On initialise le hook
    }

    void enableHook(uint8_t id) {
        hooks.at(id).setActive(true);
    }

    void disableHook(uint8_t id) {
        hooks.at(id).setActive(false);
    }

    const String getReadyHookOrder(uint8_t i) {
        hooks.at(i).setActive(false);               //Si un hook est à effectuer maintenant, il ne sera plus activable
        return hooks.at(i).getOrder();
    }
    void check(int32_t x, uint32_t y, float alpha) {
        auto start = hooks.begin();
        auto end = hooks.end();
        while (start != end )
        {
            Hook &currentHook = start->second;
            if (currentHook.isActive() && !currentHook.isReady() && currentHook.check(x, y,alpha))
            {
                currentHook.setReady();             //Les conditions du hook sont réunies !
                readyIds.push_back(start->first);	//Il faudra l'executer dès que possible
            }
            ++start;
        }
    }

    std::vector<String> executeHooks()
    {
        std::vector<String> orders;

        auto start = readyIds.begin();
        auto end = readyIds.end();
        while ( start != end )
        {
            orders.push_back(getReadyHookOrder(*start));
            hooks.at(*start).setActive(true);
            ++start;
        }
        readyIds.clear();
        return(orders);
    }

    bool hookWithId(int hookId)
    {
        auto start = hooks.begin();
        auto end = hooks.end();
        while (start != end )
        {
            if(start->first == hookId)
            {
                return true;
            }
            ++start;
        }
        return false;
    }
};



#endif //TECHTHETOWN_LOWLEVEL_HOOKLIST_H
