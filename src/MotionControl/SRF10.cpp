#include "SRF10.h"

SRF10::SRF10(uint8_t id,uint8_t rangeByte,GAIN gain){
  if( id > 0x0F ) id = 0x0F;
  real_addr = (id*2+BASE_OFFSET_ADDRESS)>>1;
  setMaxRangeByte(rangeByte);
  setMaxGain(gain);
}

bool SRF10::ping(){
  Wire.beginTransmission(real_addr);
  return !Wire.endTransmission(true);
}

void SRF10::setMaxRange(float rangeCentimeters){
  uint8_t range_value_register = (uint8_t)rangeCentimeters/4.3 -1;
  Wire.beginTransmission(real_addr);
  Wire.write(REGISTER_RANGE);
  Wire.write(range_value_register);
  Wire.endTransmission();
}

void SRF10::setMaxRangeByte(uint8_t range){
  Wire.beginTransmission(real_addr);
  Wire.write(REGISTER_RANGE);
  Wire.write(range);
  Wire.endTransmission();
}

void SRF10::setMaxGain( GAIN gain ){
  Wire.beginTransmission(real_addr);
  Wire.write(REGISTER_GAIN);
  Wire.write((uint8_t)gain);
  Wire.endTransmission();
}

void SRF10::changeIdTo(uint8_t id){

  if( id > 0x0F )id=0x0F;
  changeAddressTo(id*2+BASE_OFFSET_ADDRESS);
}

void SRF10::changeAddressTo(uint8_t addr)
{
    if( addr < 0xE0 )       addr=0xE0;
    else if(addr > 0xFE )   addr=0xFE;
    else if(addr & 0x01)    --addr;

    Wire.beginTransmission(real_addr);
    Wire.write(COMMAND_REGISTER_ADDRES);
    Wire.write(0xA0);
    Wire.endTransmission();
    Wire.beginTransmission(real_addr);
    Wire.write(COMMAND_REGISTER_ADDRES);
    Wire.write(0xAA);
    Wire.endTransmission();
    Wire.beginTransmission(real_addr);
    Wire.write(COMMAND_REGISTER_ADDRES);
    Wire.write(0xA5);
    Wire.endTransmission();
    Wire.beginTransmission(real_addr);
    real_addr = addr;
    Wire.write(COMMAND_REGISTER_ADDRES);
    Wire.write(real_addr);
    Wire.endTransmission();
    real_addr = real_addr>>1;
}

void SRF10::request()
{
  if(!waitingMeasure)
  {
    waitingMeasure = true;
    Wire.beginTransmission(real_addr);
    Wire.write(COMMAND_REGISTER_ADDRES);
    Wire.write(COMMAND_DIST_CENTIMETERS);
    Wire.endTransmission();
  }
}

bool SRF10::update(){
  uint16_t data = 42;
  if( ping() )
  {
    if( Wire.read() != 0xFF )
    {
      Wire.beginTransmission(real_addr);
      Wire.write(REGISTER_RANGE);
      Wire.endTransmission();
      Wire.requestFrom(real_addr,2);
      if( Wire.available() == 2)
      {
        data = Wire.read();
        data = data << 8;
        data = data + Wire.read();
        last_distance_measured = data;
        waitingMeasure = false;
        return true;
      }
    }
  }
  return false;
}

uint16_t SRF10::getDistance(){
  return last_distance_measured;
}
