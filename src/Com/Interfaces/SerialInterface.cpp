/**
*
*Classe g�rant la communication avec le Haut Niveau(messages arrivant, sortant)
*
*ASCII:13 = CR, 10 = LF
*
*/

#include "SerialInterface.h"

SerialInterface::SerialInterface()
{
}

bool inline SerialInterface::read_char(char & buffer)
{
	buffer = Serial.read();
	return (buffer != '\r' && buffer != '\n');
}

bool SerialInterface::read(char* order)
{
	if (available() > 0) {					//Si il y a quelque chose � lire dans le port s�rie
		char readChar;
		int i = 0;

		while (read_char(readChar) && i < RX_BUFFER_SIZE) {	//Tant qu'on n'est pas � la fin d'un message(\r)
			order[i] = readChar;
			i++;												//Au cas o� on ne re�oit jamais de terminaison
		}
		if (Serial.peek()==10) {
			read_char(readChar);								//On �limine le \n
		}
        sdLogger.logWriteReception(order);
		return (strcmp(order, ""));
	}
	else {
		return false;
	}
}

bool SerialInterface::read(int32_t& value) {
	char readValue[16];

	bool status = read(readValue);

	value = strtol(readValue, nullptr, DEC);

	return status;
}

bool SerialInterface::read(int16_t & value)
{
	char readValue[16];

	bool status = read(readValue);

	value = strtol(readValue, nullptr, DEC);

	return status;
}

bool SerialInterface::read(volatile int8_t & value)
{
	char readValue[16];

	bool status = read(readValue);

	value = strtol(readValue, nullptr, DEC);

	return status;
}

bool SerialInterface::read(float& value) {
	char readValue[16];

	bool status = read(readValue);

	value = strtof(readValue, nullptr);

	return status;
}

uint8_t inline SerialInterface::available()
{
	return Serial.available();
}

void SerialInterface::printfln(const char* message) {
	Serial.println(message);
}

void SerialInterface::printf(const char* message){
	Serial.print(message);
}
