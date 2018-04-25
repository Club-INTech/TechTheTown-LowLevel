#ifndef _SERIALHL_h
#define _SERIALHL_h

#include <Arduino.h>
#include <vector>
#include <string>

#include "Utils/Singleton.hpp"
#include "Utils/stdarg.h"
#include "Utils/defines.h"
#include "Utils/average.hpp"
#include "AbstractComInterface.h"


class SerialInterface : public AbstractComInterface
{
public:
	SerialInterface();

	/* RECEPTION */

	bool read(char*);
	bool read(int32_t&);
	bool read(int16_t&);
	bool read(volatile int8_t &);
	bool read(float&);

	void printf(const char*);
	void printfln(const char*);

private:
	bool read_char(char &);
	String data;
	uint8_t available();
};
#endif
