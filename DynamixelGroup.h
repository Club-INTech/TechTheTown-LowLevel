#ifndef DYNAMIXEL_GROUP_H
#define DYNAMIXEL_GROUP_H

#include <Arduino.h>
#include <map>
#include <utility>
#include "DynamixelInterface.h"
#include "DynamixelMotor.h"

enum DynSym
{
  BASE = 0,
  MIRROR = 1,
};

const uint16_t MAX_POSITION = 0x3FF;
const uint16_t MAX_ANGLE = 300;

class DynamixelGroup
{
  private:
    std::map< const DynamixelID , std::pair<DynamixelMotor*,const DynSym> > m_motors;
    DynamixelInterface* dyn_interface = nullptr;
  public:
    DynamixelGroup()
    {}

    DynamixelGroup( DynamixelInterface* aInterface ) : dyn_interface(aInterface)
    {}

    inline void add( DynamixelMotor* aMotor , const DynSym& aSym = DynSym::BASE )
    {
      m_motors.insert( { aMotor->id() , { aMotor , aSym } } );
    }

    inline void remove( const DynamixelID& aID )
    {
      m_motors.erase(aID);
    }

    inline void clear()
    {
      m_motors.clear();
    }

    inline void setInterface( DynamixelInterface* aInterface )
    {
      dyn_interface = aInterface;
    }

    DynamixelMotor* get( const DynamixelID& aID ) const;

    void goalPosition(const uint16_t aPosition);
  	void goalPositionWait(const uint16_t aPosition);
  	void goalPositionDegree(const uint16_t posDeg);
  	void goalPositionDegreeWait(const uint16_t posdeg);
    void speed(const uint16_t aSpeed);


};

#endif //DYNAMIXEL_GROUP_H
