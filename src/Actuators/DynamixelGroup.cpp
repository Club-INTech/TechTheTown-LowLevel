#include "DynamixelGroup.h"

DynamixelMotor* DynamixelGroup::get( const DynamixelID& aID ) const
{
  auto it = m_motors.find(aID);
  if( it != m_motors.end() )
    return it->second.first;
  else
    return nullptr;
}

void DynamixelGroup::goalPosition(const uint16_t aPosition)
{
  for( auto it : m_motors )
  {
    switch( it.second.second )
    {
      case DynSym::BASE : it.second.first->goalPosition(aPosition); break;
      case DynSym::MIRROR : it.second.first->goalPosition(MAX_POSITION-aPosition); break;
    }
  }
}

void DynamixelGroup::goalPositionDegree(const uint16_t posDeg)
{
  for( auto it : m_motors )
  {
    switch( it.second.second )
    {
      case DynSym::BASE : it.second.first->goalPositionDegree(posDeg); break;
      case DynSym::MIRROR : it.second.first->goalPositionDegree(MAX_ANGLE-posDeg); break;
    }
  }
}

void DynamixelGroup::goalPositionWait(const uint16_t aPosition)
{
  if( !m_motors.empty() )
  {
    auto it = m_motors.begin();
    while( it->second.first->currentPosition() != aPosition )
    {
    }
  }
}

void DynamixelGroup::goalPositionDegreeWait(const uint16_t posdeg)
{
  if( !m_motors.empty() )
  {
    auto it = m_motors.begin();
    while( it->second.first->currentPositionDegree() != posdeg )
    {
    }
  }
}
void DynamixelGroup::speed(const uint16_t aSpeed)
{
  for( auto it : m_motors )
    it.second.first->speed(aSpeed);
}
