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
#include "Utils/pin_mapping.h"
#include "AbstractComInterface.h"

#define PORT 23500

class EthernetInterface : public AbstractComInterface
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

	/* Attributs Ethernet */

	EthernetServer server;
	EthernetClient client;

public:
	EthernetInterface();

	bool read(char*);
	bool read(int32_t&);
	bool read(int16_t&);
	bool read(volatile int8_t &);
	bool read(float&);


	void printf(const char *);
	void printfln(const char*);

};

#endif
