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

		client = server.available();
		if (client.connected()) {
			connected = true;
			client.println("CONNECTED");
		}
	}
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

	Ethernet.begin(mac, ip, dns, gateway, subnet);
}

bool inline EthernetMgr::read_char(byte & buffer)
{
	buffer = client.read();
	return (buffer != '\r' && buffer != '\n');
}

void EthernetMgr::manageClient() {
	if (!client.connected()) {
		client = server.available();
		if (!client.connected()) {
			resetCard();		//On tente de se reconnecter en relançant la carte
			server.begin();
			delay(500);
			client = server.available();
		}
	}
}

bool EthernetMgr::read(String& order)
{
	manageClient();

	if (client.available()>0) {							//Si on est connectés et il ya des choses à lire
		byte readChar;
		order = "";
		char buffer[64] = "";
		int i = 0;

		while (read_char(readChar) && i < RX_BUFFER_SIZE) {	//Tant qu'on n'est pas à la fin d'un message(\r)
			buffer[i]=readChar;
			i++;												//Au cas où on ne reçoit jamais de terminaison
		}

		read_char(readChar);								//On élimine le \n

		order.append(buffer);
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

void EthernetMgr::sendUS(uint16_t values[])
{
	String data = "";
	char header[HEADER_LENGTH] = US_HEADER;
	for (int i = 0; i < HEADER_LENGTH;i++) {
		data.append(header[i]);
	}
	for (int i = 0; i < 4; i++) {
		data.append(values[i]);
		if (i < 3) { data.append(',');}
	}

	printfln(data.c_str());
}

void EthernetMgr::print(const char* message, ...) {
	va_list args;										//Variable contenant la liste des arguments après log (...)
	va_start(args, message);

	char logToSend[64];

	vsnprintf(logToSend, 64, message, args);			//Ajoute dans logToSend de log, en formattant avec les arguments

	client.print(logToSend);

	va_end(args);
}

void EthernetMgr::printfln(const char* message, ...) {
	va_list args;										//Variable contenant la liste des arguments après log (...)
	va_start(args, message);

	char logToSend[64];

	vsnprintf(logToSend, 64, message, args);			//Ajoute dans logToSend de log, en formattant avec les arguments

	client.println(logToSend);

	va_end(args);
}

void EthernetMgr::log(const char* log, ...) {
	char data[HEADER_LENGTH + 64] = DEBUG_HEADER;
	data[HEADER_LENGTH] = '\0';
	strcat(data, log);

	va_list args;								//Variable contenant la liste des arguments après log (...)
	va_start(args, log);

	char logToSend[HEADER_LENGTH+64];
	vsnprintf(logToSend, 64, data, args);			//Ajoute dans le buffer log, en formattant les 
	printfln(logToSend);

	va_end(args);
}

void EthernetMgr::log(const String& logMessage) {
	log(logMessage.c_str());
}