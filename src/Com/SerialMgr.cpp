/**
*
*Classe g�rant la communication avec le Haut Niveau(messages arrivant, sortant)
*
*ASCII:13 = CR, 10 = LF
*
*/

#include "SerialMgr.h"

SerialMgr::SerialMgr()
{
	sdLogger = SDLog();
}

bool inline SerialMgr::read_char(char & buffer)
{
	buffer = Serial.read();
	return (buffer != '\r' && buffer != '\n');
}

bool SerialMgr::read(char* order)
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

void SerialMgr::sendUS(const std::vector<Average<uint16_t,AVERAGE_US_SIZE>>& distances)
{
	data="";
	data.append(SENSOR_HEADER[0]);
	data.append(SENSOR_HEADER[1]);
	for( uint8_t i=0;i<distances.size();i++)
	{
		data.append(distances[i].value());
		data.append(" ");
	}
	Serial.println(data);
	sdLogger.logWrite(data);
}

void SerialMgr::sendEvent(const char* event)
{
	data = "";
	data.append(EVENT_HEADER[0]);
	data.append(EVENT_HEADER[1]);
	data.append(event);
	Serial.println(data);
	sdLogger.logWrite(data);
}

void SerialMgr::sendPosition(const float* pos){
	data = "";
	data.append(POSITION_HEADER[0]);
	data.append(POSITION_HEADER[1]);
	data.append(pos[0]);
	data.append(" ");
	data.append(pos[1]);
	data.append(" ");
	data.append(pos[2]);
	Serial.println(data);
	sdLogger.logWrite(data);
}


void SerialMgr::printfln(const char* message, ...) {
	va_list args;										//Variable contenant la liste des arguments apr�s log (...)
	va_start(args, message);

	char logToSend[64];

	vsnprintf(logToSend, 64, message, args);			//Ajoute dans logToSend de log, en formattant avec les arguments
	Serial.println(logToSend);
	sdLogger.logWrite(logToSend);

	va_end(args);
}


template void SerialMgr::print<int8_t>(int8_t value);
template void SerialMgr::println<int8_t>(int8_t value);
template void SerialMgr::print<float>(float value);
template void SerialMgr::println<float>(float value);
template void SerialMgr::print<uint32_t>(uint32_t value);
template void SerialMgr::println<uint32_t>(uint32_t value);
template void SerialMgr::print<int32_t>(int32_t value);
template void SerialMgr::println<int32_t>(int32_t value);

void SerialMgr::log(const char* log, ...) {
	char data[HEADER_LENGTH + 64];
	memcpy(data,DEBUG_HEADER,HEADER_LENGTH);
	data[HEADER_LENGTH] = '\0';

	strcat(data, log);

	va_list args;								//Variable contenant la liste des arguments apr�s log (...)
	va_start(args, log);

	char logToSend[HEADER_LENGTH+64];
	vsnprintf(logToSend, 64, data, args);			//Ajoute dans le buffer log, en formattant les
	Serial.println(logToSend);
	sdLogger.logWrite(logToSend);

	va_end(args);
}

void SerialMgr::acknowledge(const char* message, ...) {
    char data[HEADER_LENGTH + 64];
    memcpy(data,ACK_HEADER,HEADER_LENGTH);
    data[HEADER_LENGTH] = '\0';

    char formattedData[HEADER_LENGTH+64];
    strcat(data, message);

    va_list args;
    va_start(args, message);

    vsnprintf(formattedData,HEADER_LENGTH+64,data, args);
    Serial.println(formattedData);
    log(formattedData);
    sdLogger.logWrite(formattedData);
    va_end(args);
}
