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
  public:
    DynamixelGroup()
    {}

    inline void add( DynamixelMotor* aMotor , const DynSym& aSym = DynSym::BASE ) //TODO Singeproofer l'ajout d'AX
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

    DynamixelMotor* get( const DynamixelID& aID ) const;

    void goalPosition(const uint16_t aPosition);
  	void goalPositionWait(const uint16_t aPosition);
  	void goalPositionDegree(const uint16_t posDeg);
  	void goalPositionDegreeWait(const uint16_t posdeg);
    void speed(const uint16_t aSpeed);


};

#endif //DYNAMIXEL_GROUP_H
