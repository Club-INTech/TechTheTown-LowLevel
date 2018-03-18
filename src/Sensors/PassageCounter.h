#ifndef PASSAGECOUNTER_H
#define PASSAGECOUNTER_H

#include <Arduino.h>
#include "VL6180X.h"

class PassageCounter
{
    private:
        VL6180X m_sensor;
        uint16_t m_count = 0;
        uint8_t m_threshold_mm = 20;
        uint8_t m_delta_threshold_mm;
        bool m_detection = false;

    public:
        PassageCounter(uint8_t threshold_mm = 20 , uint8_t delta_threshold_mm = 0 , int8_t offset_mm = 0 );
        bool update();
        void enableContinuousUpdate(uint16_t period);
        inline void reset(){ m_count = 0; }
        inline uint16_t getCount() const { return m_count; }
};

#endif //PASSAGECOUNTER_H
