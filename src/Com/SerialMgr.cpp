/**
*
*Classe gérant la communication avec le Haut Niveau(messages arrivant, sortant)
*
*ASCII:13 = CR, 10 = LF
*
*/

#include "SerialMgr.h"

SerialMgr::SerialMgr()
{
}

bool inline SerialMgr::read_char(char & buffer)
{
	buffer = Serial.read();
	return (buffer != '\r' && buffer != '\n');
}

bool SerialMgr::read(char* order)
{
	if (available() > 0) {					//Si il y a quelque chose à lire dans le port série
		char readChar;
		int i = 0;

		while (read_char(readChar) && i < RX_BUFFER_SIZE) {	//Tant qu'on n'est pas à la fin d'un message(\r)
			order[i] = readChar;
			i++;												//Au cas où on ne reçoit jamais de terminaison
		}
		if (Serial.peek()==10) {
			read_char(readChar);								//On élimine le \n
		}
		return (strcmp(order, ""));
	}
	else {
		return false;
	}
}

bool SerialMgr::read(int32_t& value) {
	char readValue[16];

	bool status = read(readValue);

	value = strtol(readValue, nullptr, DEC);

	return status;
}

bool SerialMgr::read(int16_t & value)
{
	char readValue[16];

	bool status = read(readValue);

	value = strtol(readValue, nullptr, DEC);

	return status;
}

bool SerialMgr::read(volatile int8_t & value)
{
	char readValue[16];

	bool status = read(readValue);

	value = strtol(readValue, nullptr, DEC);

	return status;
}

bool SerialMgr::read(float& value) {
	char readValue[16];

	bool status = read(readValue);

	value = strtof(readValue, nullptr);
	
	return status;
}

uint8_t inline SerialMgr::available()
{
	return Serial.available();
}

void SerialMgr::sendUS(uint16_t value)
{
	String data = "";
	char header[HEADER_LENGTH] = SENSOR_HEADER;
	for (int i = 0; i < HEADER_LENGTH;i++) {
		data.append(header[i]);
	}
	data.append(value);
	Serial.println(data);
}

void SerialMgr::sendEvent(const char* event) 
{
	String data = "";
	char header[HEADER_LENGTH] = EVENT_HEADER;
	for (int i = 0; i < HEADER_LENGTH; i++) {
		data.append(header[i]);
	}
	data.append(event);
	Serial.println(data);
}

template void SerialMgr::print<int8_t>(int8_t value);
template void SerialMgr::println<int8_t>(int8_t value);
template void SerialMgr::print<float>(float value);
template void SerialMgr::println<float>(float value);
template void SerialMgr::print<uint32_t>(uint32_t value);
template void SerialMgr::println<uint32_t>(uint32_t value);
template void SerialMgr::print<int32_t>(int32_t value);
template void SerialMgr::println<int32_t>(int32_t value);

void SerialMgr::printfln(const char* message, ...) {
	va_list args;										//Variable contenant la liste des arguments après log (...)
	va_start(args, message);

	char logToSend[64];

	vsnprintf(logToSend, 64, message, args);			//Ajoute dans logToSend de log, en formattant avec les arguments
	Serial.println(logToSend);

	va_end(args);
}

void SerialMgr::log(const char* log, ...) {
	char data[HEADER_LENGTH + 64] = DEBUG_HEADER;
	data[HEADER_LENGTH] = '\0';

	strcat(data, log);

	va_list args;								//Variable contenant la liste des arguments après log (...)
	va_start(args, log);

	char logToSend[HEADER_LENGTH+64];
	vsnprintf(logToSend, 64, data, args);			//Ajoute dans le buffer log, en formattant les 
	Serial.println(logToSend);

	va_end(args);
}