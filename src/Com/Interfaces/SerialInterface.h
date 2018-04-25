#ifndef _SERIALHL_h
#define _SERIALHL_h

#include "../../../../../../.platformio/packages/framework-arduinoteensy/cores/teensy3/Arduino.h"
#include "../../../../../../.platformio/packages/toolchain-gccarmnoneeabi/arm-none-eabi/include/c++/5.4.1/vector"
#include "../../../../../../.platformio/packages/toolchain-gccarmnoneeabi/arm-none-eabi/include/c++/5.4.1/string"

#include "../../Utils/Singleton.hpp"
#include "../../Utils/stdarg.h"
#include "../../Utils/defines.h"
#include "../../Utils/average.hpp"
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
