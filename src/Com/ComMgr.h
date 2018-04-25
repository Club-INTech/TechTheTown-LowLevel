//
// Created by asphox on 24/04/18.
//

#ifndef TECHTHETOWN_LOWLEVEL_COMMGR_H
#define TECHTHETOWN_LOWLEVEL_COMMGR_H

#include "Interfaces/SerialInterface.h"
#include "Interfaces/EthernetInterface.h"
#include "Utils/Singleton.hpp"
#include "Utils/define_com_options.h"
#include <map>

class ComMgr : public Singleton<ComMgr>
{
public:

    ComMgr();
    ~ComMgr();

    template< typename T >
    bool read(T data){
        static bool r1=true,r2=true;
        if( com_options & COM_OPTIONS::ETHERNET_R )
            r1 = ethernet->read(data);
        if( com_options & COM_OPTIONS::SERIAL_R )
            r2 = serial->read(data);
        return r1&&r2;
    }

    /* ENVOI */
    void sendUS(const std::vector<Average<uint16_t,AVERAGE_US_SIZE>>&);
    void sendEvent(const char*);
    void sendPosition(const float*);

    void acknowledge(const char*);
    void removeEventsToAcknowledge(const char *ackID);
    void sendEventsToAcknowledge();

    void printfln(Header header,const char*, ...) __attribute__((format(printf, 3, 4)));
    void printf(Header header,const char*,...) __attribute__((format(printf, 3, 4)));

    void startMatch();

private:
    AbstractComInterface*    ethernet = nullptr;
    AbstractComInterface*      serial = nullptr;
    SDLog sdlog;
    std::map<int, char*> eventsToAcknowledge;
    int volatile currentAckID;

    void addEventsToAcknowledge(const char *ackID, const char *waitingForAckEvent);

};


#endif //TECHTHETOWN_LOWLEVEL_COMMGR_H
