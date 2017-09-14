#ifndef _SERIALHL_h
#define _SERIALHL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif

#include "Singleton.hpp"
#include "stdarg.h"
#include <WString.h>
#include "defines.h"

#define RX_TIMEOUT 1000 //TODO gérer les timeouts(surtout pour le moniteur série..)

#define HEADER_LENGTH   2

#define DEBUG_HEADER    {0x02, 0x20}
#define EVENT_HEADER    {0x13, 0x37}
#define US_HEADER       {0x01, 0x10}

class SerialHL : public Singleton<SerialHL>
{
public:

	SerialHL();
	enum {
		READ_TIMEOUT = 0, READ_SUCCESS = 1
	};

	/* RECEPTION */
	bool read_char(char &);
	bool read(String&, bool wait = false);
	bool read(uint32_t&, bool wait = false);
	bool read(int16_t&, bool wait = false);
	bool read(volatile int8_t &, bool wait = false);
	bool read(float&, bool wait = false);
	uint8_t available();

	/* ENVOI */
	void sendUS(uint16_t);
	void printfln(const char*, ...) __attribute__((format(printf, 2, 3)));
	void log(const char*, ...) __attribute__((format(printf, 2, 3)));
};
#endif