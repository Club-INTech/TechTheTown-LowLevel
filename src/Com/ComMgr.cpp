//
// Created by asphox on 24/04/18.
//

#include "ComMgr.h"

ComMgr::ComMgr()
{
    ethernet = new EthernetInterface();
    serial = new SerialInterface();
}

void ComMgr::sendUS(const std::vector<Average<uint16_t, AVERAGE_US_SIZE>> & data)
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
    char strCurrentAckID[4];
    sprintf(strCurrentAckID,"%4d",currentAckID);

    char formatted[64];
    memcpy(formatted,strCurrentAckID,4);
    formatted[4]='\0';
    strcat(formatted+4,data);

    printfln(EVENT_HEADER,formatted);
    addEventsToAcknowledge(strCurrentAckID,formatted);
    currentAckID++;
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

void ComMgr::acknowledge(const char * data)
{
    printfln(ACK_HEADER,data);
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


/**
 *  Ajoute un event à la liste des events en attente d'acknowledgement
 */
void ComMgr::addEventsToAcknowledge(const char* ackID, const char* waitingForAckEvent)
{
    eventsToAcknowledge[atoi(ackID)]=waitingForAckEvent;
}

/**
 *  Enlève un event à la liste des events en attente d'acknowledgement
 */
void ComMgr::removeEventsToAcknowledge(const char* ackID)
{
    eventsToAcknowledge.erase(atoi(ackID));
}

/**
 * Renvoie les events qui n'ont pas encore reçu d'acknowledge
 */
void ComMgr::sendEventsToAcknowledge()
{
    std::map<int, const char*>::iterator it = eventsToAcknowledge.begin();
    while (it != eventsToAcknowledge.end()){
        printfln(EVENT_HEADER,it->second);
        it++;
    }
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


