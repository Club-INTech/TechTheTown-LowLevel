#include "PassageCounter.h"

PassageCounter::PassageCounter(uint8_t threshold_mm , uint8_t delta_threshold_mm , int8_t offset ) :
m_threshold_mm(threshold_mm),
m_delta_threshold_mm(delta_threshold_mm)
{
    m_sensor.init();
    m_sensor.configureDefault();
    m_sensor.setSoftwareOffset(offset);
}

bool PassageCounter::update()
{
    uint8_t last_measure = m_sensor.readRangeSingleMillimeters();
    if( last_measure <= m_threshold_mm && !m_detection)
    {
        m_detection=true;
        ++m_count;
        return true;
    }
    else if( last_measure <= m_threshold_mm && m_detection)
    {
        return true;
    }
    else if( last_measure > m_threshold_mm+m_delta_threshold_mm && m_detection)
    {
        m_detection=false;
    }
    return false;
}
