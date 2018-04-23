//
// Created by Teo-CD on 22/04/18.
//

#ifndef LL_SDLOG_H
#define LL_SDLOG_H

#include "SD.h"
#include "Utils/defines.h"

class SDLog {

public:
    SDLog();

    bool logWrite(String);
    bool logWrite(const char*);
    bool logWriteReception(const char*);

    void setStartingTime();

private:
    const char* headerList[6]{STD_HEADER,DEBUG_HEADER,EVENT_HEADER,SENSOR_HEADER,POSITION_HEADER,ACK_HEADER};
    const char* fileList[8]{"/full.txt","/input.txt","/standard.txt","/debug.txt","/event.txt","/sensor.txt","/position.txt","/ack.txt"};

    uint32_t startingTime;

    bool logToFile(const String&,const char*);
};


#endif //LL_SDLOG_H
