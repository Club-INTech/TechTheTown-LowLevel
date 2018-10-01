//
// Created by asphox on 28/09/18.
//

#ifndef TECHTHETOWN_LOWLEVEL_MONITORING_H
#define TECHTHETOWN_LOWLEVEL_MONITORING_H

#include <cstdint>
#include <malloc.h>
#include <Arduino.h>

extern byte _estack;
extern byte* __brkval;

class Monitoring
{
private:
    static constexpr uint32_t RAM_SIZE_BYTE = 196608;
public:
    static uintptr_t stackSize()
    {
        byte _tstack;
        return &_estack - &_tstack;
    }

    static intptr_t freeRAM()
    {
        byte _tstack;
        return &_tstack - __brkval;
    }

    inline static float freeRAMPercent()
    {
        return ((float)freeRAM()/(float)RAM_SIZE_BYTE)*100;
    }

    inline static uintptr_t RAMUsage()
    {
        return RAM_SIZE_BYTE-freeRAM();
    }

    inline static float RAMUsagePercent()
    {
        return ((float)RAM_SIZE_BYTE-(float)freeRAM())/RAM_SIZE_BYTE*100;
    }

    inline static uintptr_t heapSize()
    {
        return mallinfo().uordblks;
    }
};

#endif //TECHTHETOWN_LOWLEVEL_MONITORING_H
