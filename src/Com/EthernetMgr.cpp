/**
*
*Classe g�rant la communication avec le Haut Niveau(messages arrivant, sortant)
*
*ASCII:13 = CR, 10 = LF
*/

#include "EthernetMgr.h"

EthernetMgr::EthernetMgr():server{ PORT }
{
	resetCard();

	if (Ethernet.localIP() != ip) {
		Serial.println("ERR\tIP CONFIGURATON FAILED");
		connected = false;
	}
	else {
		Serial.print("Ethernet Ready\nLocal ip: ");
		Serial.println(Ethernet.localIP());
		server.begin();
		delay(1000);
		client = server.available();
		if (client.connected()) {
			connected = true;
			client.println("CONNECTED");
		}
	}
	lastMessage = millis();
}

void EthernetMgr::resetCard() {

	Serial.println("Resetting WIZ820io");

	pinMode(PWD, OUTPUT);
	digitalWrite(PWD, HIGH);
	delay(10);
	digitalWrite(PWD, LOW);

	pinMode(RST, OUTPUT);
	digitalWrite(RST, LOW);
	delayMicroseconds(3);
	digitalWrite(RST, HIGH);
	delay(150);

	Ethernet.begin(mac, ip, dns, gateway, subnet);
}
bool inline EthernetMgr::read_char(char & buffer)
{
	buffer = client.read();
	return (buffer != '\r' && buffer != '\n');
}

bool EthernetMgr::read(char* order)
{
	//manageClient();
	client = server.available();

	if (client) {							//Si on est connect�s et il ya des choses � lire
		char readChar;
		int i = 0;

		while (read_char(readChar) && i < RX_BUFFER_SIZE) {	//Tant qu'on n'est pas � la fin d'un message(\r)
			order[i] = readChar;
			i++;												//Au cas o� on ne re�oit jamais de terminaison, on limite le nombre de chars
		}
		if (client.peek() == 10) {
			read_char(readChar);		//On �limine le \n terminal
		}
		lastMessage = millis();
		return (strcmp(order, ""));
	}
	else {
		return false;
	}

}

bool EthernetMgr::read(int32_t & value)
{
	char readValue[16];

	bool status = read(readValue);

	value = strtol(readValue, nullptr, DEC);

	return status;
}

bool EthernetMgr::read(int16_t & value)
{
	char readValue[16];

	bool status = read(readValue);

	value = strtol(readValue, nullptr, DEC);

	return status;
}

bool EthernetMgr::read(volatile int8_t & value)
{
	char readValue[16] = "";

	bool status = read(readValue);

	value = strtol(readValue, nullptr, DEC);

	return status;
}

bool EthernetMgr::read(float& value) {
	char readValue[16] = "";

	bool status = read(readValue);

	value = strtof(readValue, nullptr);

	return status;
}

/**
*	Envoie une chaine de caract�res commen�ant par 2 headers Ultrason, puis les valeurs s�par�es par des virgules
*/
void EthernetMgr::sendUS(uint16_t values[])
{
	char valueString[HEADER_LENGTH] = SENSOR_HEADER;
	char currentValue[4] = "";	//Comment g�rer des values de tailles diff�rentes?

	for (int i = 0; i < 3; ++i) {
		itoa(values[i], currentValue, DEC);
		strcat(valueString, currentValue);
		strcat(valueString, ",");
	}
	itoa(values[3], currentValue, DEC);
	strcat(valueString, currentValue);

	printfln(valueString);
}

void EthernetMgr::sendEvent(const char* event)
{
	String data = "";
	char header[HEADER_LENGTH] = EVENT_HEADER;
	for (int i = 0; i < HEADER_LENGTH; i++) {
		data.append(header[i]);
	}
	data.append(event);
	printfln(data.c_str());
}

void EthernetMgr::sendPosition(const float* pos)
{
	String data = "";
	char header[HEADER_LENGTH] = POSITION_HEADER;
	for (int i = 0; i < HEADER_LENGTH; i++) {
		data.append(header[i]);
	}
	for (int i=0; i<3; i++){
		data.append(pos[i]);
		data.append(" ");
	}
	printfln(data.c_str());
}

void EthernetMgr::print(const char* message, ...) {
	va_list args;										//Variable contenant la liste des arguments apr�s log (...)
	va_start(args, message);

	char logToSend[64];

	vsnprintf(logToSend, 64, message, args);			//Ajoute dans logToSend de log, en formattant avec les arguments
	
	client.print(logToSend);

	va_end(args);
}

template void EthernetMgr::print<int8_t>(int8_t value);
template void EthernetMgr::println<int8_t>(int8_t value);
template void EthernetMgr::print<float>(float value);
template void EthernetMgr::println<float>(float value);
template void EthernetMgr::print<uint32_t>(uint32_t value);
template void EthernetMgr::println<uint32_t>(uint32_t value);
template void EthernetMgr::print<int32_t>(int32_t value);
template void EthernetMgr::println<int32_t>(int32_t value);

void inline EthernetMgr::printfln(const char* message, ...) {
	va_list args;
	va_start(args, message);

	print(message, args);
	client.print("\r\n");

	va_end(args);
}

void EthernetMgr::log(const char* log, ...) {
	char data[HEADER_LENGTH + 64] = DEBUG_HEADER;
	data[HEADER_LENGTH] = '\0';

	strcat(data, log);

	va_list args;								//Variable contenant la liste des arguments apr�s log (...)
	va_start(args, log);

	char logToSend[HEADER_LENGTH + 64];
	vsnprintf(logToSend, 64, data, args);			//Ajoute dans le buffer log, en formattant les 
	client.println(logToSend);

	va_end(args);
}