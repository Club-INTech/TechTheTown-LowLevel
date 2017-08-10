/**
*
*Classe gérant la communication avec le Haut Niveau(messages arrivant, sortant)
*
*ASCII:13 = CR, 10 = LF
*/

#include "EthernetMgr.h"


EthernetMgr::EthernetMgr()
{
	uint8_t mac[6]{ 0x04, 0xE9, 0xE5, 0x04, 0xE9, 0xE5 };  //Addresse mac de la Teensy, ne pas changer
	IPAddress ip{ 192,168,0,1 };
	IPAddress dns{ 8,8,8,8 };
	IPAddress gateway{ 192,168,0,2 };
	IPAddress subnet{ 255,255,255,0 };

	resetCard();

	Ethernet.begin(mac, ip, dns, gateway, subnet);

	Serial.print("Ethernet Ready\nLocal ip: ");
	Serial.println(Ethernet.localIP());
	server.begin();
	client = server.available();
}

void EthernetMgr::resetCard() {
	pinMode(PWD, OUTPUT);
	digitalWrite(PWD, HIGH);
	delay(10);
	digitalWrite(PWD, LOW);

	pinMode(RST, OUTPUT);
	digitalWrite(RST, LOW);
	delayMicroseconds(3);
	digitalWrite(RST, HIGH);
	delay(150);
}

bool EthernetMgr::connected() {
	return client.connected();
}

bool inline EthernetMgr::read_char(char & buffer)
{
	buffer = client.read();
	return (buffer != '\r');
}

bool EthernetMgr::read(String& order)
{
	uint32_t start = micros();
	order = "";
	char buffer[64];
	client = server.available();
	char readChar;
	int i = 0;
	if (client) {							//Si on est connectés et il ya des choses à lire
		while (read_char(readChar) && i < RX_BUFFER_SIZE) {	//Tant qu'on n'est pas à la fin d'un message(\r)
			order.append(readChar);
			i++;												//Au cas où on ne reçoit jamais de terminaison
		}
		if (client) {
			read_char(readChar);								//On élimine le \n
		}
		Serial.println(micros()- start);
		Serial.println(order);
		return (!order.equals(""));
	}
	else {
		return false;
	}
	
	
}

bool EthernetMgr::read(int16_t & value)
{
	String readValue = "";

	bool status = read(readValue);

	value = strtol(readValue.c_str(), nullptr, DEC);

	return status;
}

bool EthernetMgr::read(volatile int8_t & value)
{
	String readValue = "";

	bool status = read(readValue);

	value = strtol(readValue.c_str(), nullptr, DEC);

	return status;
}

bool EthernetMgr::read(float& value) {
	String readValue = "";

	bool status = read(readValue);

	value = strtof(readValue.c_str(), nullptr);
	
	return status;
}

void EthernetMgr::sendUS(uint16_t value)
{
	String data = "";
	char header[HEADER_LENGTH] = US_HEADER;
	Serial.println("us");
	for (int i = 0; i < HEADER_LENGTH;i++) {
		data.append(header[i]);
	}

	data.append(value);

	Serial.println(data);
}

void EthernetMgr::printfln(const char* message, ...) {
	va_list args;										//Variable contenant la liste des arguments après log (...)
	va_start(args, message);

	char logToSend[64];

	vsnprintf(logToSend, 64, message, args);			//Ajoute dans logToSend de log, en formattant avec les arguments

	Serial.println(logToSend);

	va_end(args);
}

void EthernetMgr::printfLog(const char* log, ...) {
	char data[HEADER_LENGTH + 64] = DEBUG_HEADER;
	data[HEADER_LENGTH] = '\0';
	Serial.println("log");
	strcat(data, log);

	va_list args;								//Variable contenant la liste des arguments après log (...)
	va_start(args, log);

	char logToSend[HEADER_LENGTH+64];
	vsnprintf(logToSend, 64, data, args);			//Ajoute dans le buffer log, en formattant les 
	Serial.println(logToSend);

	va_end(args);
}