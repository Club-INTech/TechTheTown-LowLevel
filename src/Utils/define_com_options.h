//
// Created by asphox on 25/04/18.
//

#ifndef TECHTHETOWN_LOWLEVEL_DEFINE_COM_OPTIONS_H
#define TECHTHETOWN_LOWLEVEL_DEFINE_COM_OPTIONS_H

enum COM_OPTIONS
{
    SERIAL_W    = 0b1,
    SERIAL_R    = 0b10,
    ETHERNET_W  = 0b100,
    ETHERNET_R  = 0b1000,

    ETHERNET_RW          = ETHERNET_W|ETHERNET_R,
    SERIAL_RW            = SERIAL_W|SERIAL_R,
    ETHERNET_RW_SERIAL_W   = ETHERNET_RW|SERIAL_W

};
inline COM_OPTIONS operator|(COM_OPTIONS a, COM_OPTIONS b)
{return static_cast<COM_OPTIONS >(static_cast<int>(a) | static_cast<int>(b));}

//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////
constexpr COM_OPTIONS com_options = ETHERNET_RW_SERIAL_W;
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


#endif //TECHTHETOWN_LOWLEVEL_DEFINE_COM_OPTIONS_H
