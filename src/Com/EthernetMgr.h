/**		new/old

SS		10/10
INT		///
RST		9/14   /!\ PIN 24 strap sur PIN9 -> 24 en INPUT
PWDN	24/15
MISO	12/12
MOSI	11/11

*/

#ifndef _ETHERNETMGR_h
#define _ETHERNETMGR_h

#include <Arduino.h>
#include <vector>
#include "Utils/Singleton.hpp"
#include "Utils/stdarg.h"
#include <WString.h>
#include "Utils/defines.h"
#include <SPI.h>
#include <Ethernet.h>
#include "MotionControl/MotionControlSystem.h"
#include "Utils/pin_mapping_select.h"

#define PORT 23500

class EthernetMgr : public Singleton<EthernetMgr>
{
private:
	//Param�tres TCP/IP
	uint8_t mac[6]{ 0x04, 0xE9, 0xE5, 0x04, 0xE9, 0xE5 };  //Addresse mac de la Teensy, ne pas changer
	IPAddress ip{ 192,168,0,1 };
	IPAddress dns{ 8,8,8,8 };
	IPAddress gateway{ 192,168,0,2 };
	IPAddress subnet{ 255,255,255,0 };

	String valueString;

	void resetCard();

	bool read_char(char & buffer);

	/* Attributs Ethernet */

	EthernetServer server;
	EthernetClient client;

public:
	EthernetMgr();

	/* RECEPTION */

	bool read(char*);
	bool read(int32_t&);
	bool read(int16_t&);
	bool read(volatile int8_t &);
	bool read(float&);

	/* ENVOI */
	void sendUS(const std::vector<uint16_t>&);
	void sendEvent(const char*);
	void sendPosition(const float*);
    void log(const char*, ...) __attribute__((format(printf, 2, 3)));

	void printf(const char *, ...) __attribute__((format(printf, 2, 3)));
	void printfln(const char*, ...) __attribute__((format(printf, 2, 3)));

    template<typename T>
    inline void print(T value) {
        client.print(value);
    }
    template<typename T>
    inline void println(T value) {
        client.println(value);
    }

};

template void EthernetMgr::print<int8_t>(int8_t value);
template void EthernetMgr::println<int8_t>(int8_t value);
template void EthernetMgr::print<float>(float value);
template void EthernetMgr::println<float>(float value);
template void EthernetMgr::print<uint32_t>(uint32_t value);
template void EthernetMgr::println<uint32_t>(uint32_t value);
template void EthernetMgr::print<int32_t>(int32_t value);
template void EthernetMgr::println<int32_t>(int32_t value);

#endif
