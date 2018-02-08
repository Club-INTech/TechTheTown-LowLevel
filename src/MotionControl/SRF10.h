#ifndef SRF10_H
#define SRF10_H

#include <Arduino.h>
#include <i2c_t3.h>
#include "AbstractSensorUS.h"

class SRF10 : public AbstractSensorUS
{
private:
    uint8_t real_addr = BASE_OFFSET_ADDRESS;
    uint16_t last_distance_measured = 0xFFFF;
    bool waitingMeasure = false;

  public:

    static constexpr uint8_t BASE_OFFSET_ADDRESS = 0xE0;
    static constexpr uint8_t COMMAND_REGISTER_ADDRES = 0x00;
    static constexpr uint8_t REGISTER_REVISION = 0x00;
    static constexpr uint8_t REGISTER_GAIN = 0x01;
    static constexpr uint8_t REGISTER_RANGE = 0x02;
    static constexpr uint8_t COMMAND_DIST_INCHES = 0x50;
    static constexpr uint8_t COMMAND_DIST_CENTIMETERS = 0x51;
    static constexpr uint8_t COMMAND_DIST_MICROSECONDS = 0x52;

    static enum GAIN{
      G40 = 0x00,
      G50 = 0x02,
      G60 = 0x03,
      G70 = 0x04,
      G80 = 0x05,
      G100 = 0x06,
      G120 = 0x07,
      G140 = 0x08,
      G200 = 0x09,
      G250 = 0x0A,
      G300 = 0x0B,
      G350 = 0x0C,
      G400 = 0x0D,
      G500 = 0x0E,
      G600 = 0x0F,
      G700 = 0x10
    } Gain;

    SRF10(uint8_t addr,uint8_t rangeByte,GAIN gain);  /* 0 <= addr < 16 */
    /*GAIN 100 et range 255 good */

    bool ping();

    void setMaxRange(float rangeCentimeters);
    void setMaxRangeByte(uint8_t range);
    void setMaxGain(GAIN gain);

    void changeAddressTo(uint8_t new_addr);

    void request();
    bool update();
    uint16_t getDistance();

};

#endif //SRF10_H
