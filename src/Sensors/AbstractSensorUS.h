#ifndef ABSTRACTSENSORUS_H
#define ABSTRACTSENSORUS_H

#include <i2c_t3.h>

class AbstractI2CSensorUS
{
  public:
    virtual uint16_t getDistance() = 0;
    virtual bool     update() = 0;
    virtual void     request() = 0;
    virtual bool     ping() = 0;
};

#endif //ABSTRACTSENSORUS_H
