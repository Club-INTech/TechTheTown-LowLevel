/**
*
*Classe gérant la communication avec le Haut Niveau(messages arrivant, sortant)
*
*ASCII:13 = CR, 10 = LF
*/

#include "EthernetMgr.h"

EthernetMgr::EthernetMgr()
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
	Serial.println(buffer);
	return (buffer != '\r' && buffer != '\n');
}

bool EthernetMgr::read(char* order)
{
	//manageClient();
	client = server.available();

	if (client) {							//Si on est connectés et il ya des choses à lire
		char readChar;
		int i = 0;

		while (read_char(readChar) && i < RX_BUFFER_SIZE) {	//Tant qu'on n'est pas à la fin d'un message(\r)
			order[i] = readChar;
			i++;												//Au cas où on ne reçoit jamais de terminaison, on limite le nombre de chars
		}
		if (client.peek() == 10) {
			read_char(readChar);		//On élimine le \n terminal
		}
		lastMessage = millis();
		Serial.println(order);
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
*	Envoie une chaine de caractères commençant par 2 headers Ultrason, puis les valeurs séparées par des virgules
*/
void EthernetMgr::sendUS(uint16_t values[])
{
	char valueString[HEADER_LENGTH] = SENSOR_HEADER;
	char currentValue[4] = "";	//Comment gérer des values de tailles différentes?

	for (int i = 0; i < 3; ++i) {
		itoa(values[i], currentValue, DEC);
		strcat(valueString, currentValue);
		strcat(valueString, ",");
	}
	itoa(values[3], currentValue, DEC);
	strcat(valueString, currentValue);

	printfln(valueString);
}

void inline EthernetMgr::print(const char* message, ...) {
	va_list args;										//Variable contenant la liste des arguments après le log
	va_start(args, message);

	char logToSend[64];

	vsnprintf(logToSend, 64, message, args);			//Ajoute dans logToSend de log, en formattant avec les arguments

	client.print(logToSend);


	va_end(args);
}

void inline EthernetMgr::printfln(const char* message, ...) {
	va_list args;
	va_start(args, message);
	log(message, args);
	va_end(args);
}

void inline EthernetMgr::log(const char* log, ...) {
	char data[HEADER_LENGTH + 64] = DEBUG_HEADER;
	data[HEADER_LENGTH] = '\0';
	strcat(data, log);

	va_list args;								//Variable contenant la liste des arguments après log (...)
	va_start(args, log);

	char logToSend[HEADER_LENGTH + 64];
	vsnprintf(logToSend, 64, data, args);			//Ajoute dans le buffer log, en formattant les 
	printfln(logToSend);

	va_end(args);
}