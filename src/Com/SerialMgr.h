#ifndef _SERIALHL_h
#define _SERIALHL_h

#include <Arduino.h>
#include <vector>
#include <string>

#include "Utils/Singleton.hpp"
#include "Utils/stdarg.h"
#include "Utils/defines.h"
#include "Utils/average.hpp"



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
	void sendUS(const std::vector<Average<uint16_t,AVERAGE_US_SIZE>>&);
	void sendEvent(const char*);

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
	String data;
	uint8_t available();

    void sendPosition(const float *pos);
};
#endif
