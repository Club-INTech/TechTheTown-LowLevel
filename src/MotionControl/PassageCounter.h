#ifndef PASSAGECOUNTER_H
#define PASSAGECOUNTER_H

#include <Arduino.h>
#include "VL6180X.h"

class PassageCounter
{
    private:
        VL6180X m_sensor;
        uint8_t m_count = 0;
        uint8_t m_threshold_mm = 20;
        uint8_t m_delta_threshold_mm;
        bool m_detection = false;
        void(*m_onIncrement)();

        static inline void do_nothing(){}

    public:
        PassageCounter(uint8_t threshold_mm = 20 , uint8_t delta_threshold_mm = 0 , void(*)() = do_nothing );
        void update();
        inline void reset(){ m_count = 0; }
        inline uint8_t getCount() const { return m_count; }
};

#endif //PASSAGECOUNTER_H
