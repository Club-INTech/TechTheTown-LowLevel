#ifndef _SERIALHL_h
#define _SERIALHL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif

#include "Singleton.hpp"
#include "stdarg.h"
#include "defines.h"



class SerialMgr : public Singleton<SerialMgr>
{
public:
	SerialMgr();

	/* RECEPTION */
	
	bool read(char*);
	bool read(int32_t&);
	bool read(int16_t&);
	bool read(volatile int8_t &);
	bool read(float&);
	
	/* ENVOI */
	void sendUS(uint16_t);

	template<typename T>
	void print(T value) {
		Serial.print(value);
	}
	template<typename T>
	void println(T value) {
		Serial.println(value);
	}

	void print(const char*, ...) __attribute__((format(printf, 2, 3)));
	void printfln(const char*, ...) __attribute__((format(printf, 2, 3)));
	void log(const char*, ...) __attribute__((format(printf, 2, 3)));

private:
	bool read_char(char &);
	uint8_t available();
};
#endif