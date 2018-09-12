//
// Created by asphox on 24/04/18.
//

#include "ComMgr.h"

ComMgr::ComMgr()
{
    if( com_options & ETHERNET_RW )
    {
        ethernet = new EthernetInterface();
    }
    serial = new SerialInterface();
}

void ComMgr::sendUS(const std::array<Median<uint16_t, MEDIAN_US_SIZE>,NBR_OF_US_SENSOR> & data)
{
    String tmp = "";
    for(uint8_t i=0;i<data.size();i++){
        tmp.append(data[i].value());
        tmp.append(" ");
    }
    printfln(SENSOR_HEADER,tmp.c_str());
}

void ComMgr::sendEvent(const char * data)
{
    char formatted[64];
    strcat(formatted,data);
    printfln(EVENT_HEADER,formatted);
}

void ComMgr::sendPosition(const float * data)
{
    String tmp="";
    for(int i=0;i<3;i++){
        tmp.append(data[i]);
        tmp.append(" ");
    }
    printfln(POSITION_HEADER,tmp.c_str());
}

void ComMgr::printfln(Header header,const char * data,...)
{
    va_list args;
    va_start(args,data);

    char tmp[HEADER_LENGTH + 64];
    memcpy(tmp,header,HEADER_LENGTH);
    tmp[HEADER_LENGTH] = '\0';

    strcat(tmp+HEADER_LENGTH, data);
    char formatted[HEADER_LENGTH + 64];
    vsnprintf(formatted, HEADER_LENGTH+64, tmp, args);

    if( com_options & COM_OPTIONS::ETHERNET_W )
        ethernet->printfln(formatted);
    if( com_options & COM_OPTIONS::SERIAL_W )
        serial->printfln(formatted);

    if( memcmp(header,DEBUG_HEADER,HEADER_LENGTH) )
        printfln(DEBUG_HEADER,formatted);

    sdlog.logWrite(formatted);

    va_end(args);
}

void ComMgr::printf(Header header, const char *data, ...)
{
    va_list args;
    va_start(args,data);

    char tmp[HEADER_LENGTH + 64];
    memcpy(tmp,header,HEADER_LENGTH);
    tmp[HEADER_LENGTH] = '\0';

    strcat(tmp+HEADER_LENGTH, data);
    char formatted[HEADER_LENGTH + 64];
    vsnprintf(formatted, HEADER_LENGTH+64, tmp, args);

    if( com_options & COM_OPTIONS::ETHERNET_W )
        ethernet->printf(formatted);
    if( com_options & COM_OPTIONS::SERIAL_W )
        serial->printf(formatted);

    if( memcmp(header,DEBUG_HEADER,HEADER_LENGTH) )
        printfln(DEBUG_HEADER,formatted);

    sdlog.logWrite(formatted);

    va_end(args);
}

void ComMgr::startMatch()
{
    sendEvent("BLITZKRIEG");
    sdlog.setStartingTime();
}

ComMgr::~ComMgr()
{
    delete static_cast<EthernetInterface*>(ethernet);
    delete static_cast<SerialInterface*>(serial);
}


