/**
SS		10
INT		/
RST		14
PWDN	15
MISO	12
MOSI	11
*/


#ifndef _ETHERNETMGR_h
#define _ETHERNETMGR_h

#include <Arduino.h>


#include "Utils/Singleton.hpp"
#include "Utils/stdarg.h"
#include <WString.h>
#include "Utils/defines.h"
#include <SPI.h>
#include <Ethernet.h>
#include "MotionControl/MotionControlSystem.h"
#include "Utils/pin_mapping.h"



#define CONNECTION_TIMEOUT	1000

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

	void resetCard();

	bool read_char(char & buffer);

	uint32_t lastMessage = 0;

	/* Attributs Ethernet */

	EthernetServer server;
	EthernetClient client;

public:
	EthernetMgr();

	bool connected;

	/* RECEPTION */

	bool read(char*);
	bool read(int32_t&);
	bool read(int16_t&);
	bool read(volatile int8_t &);
	bool read(float&);

	/* ENVOI */
	void sendUS(uint16_t[]);
	void sendEvent(const char*);

	template<typename T>
	void print(T value) {
		client.print(value);
	}
	template<typename T>
	void println(T value) {
		client.println(value);
	}

	void print(const char*, ...) __attribute__((format(printf, 2, 3)));
	void printfln(const char*, ...) __attribute__((format(printf, 2, 3)));
	void log(const char*, ...) __attribute__((format(printf, 2, 3)));
};
#endif