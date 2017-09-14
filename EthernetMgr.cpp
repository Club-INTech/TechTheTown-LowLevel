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
	buffer = (char)client.read();
	return (buffer != '\r' && buffer != '\n');
}

void EthernetMgr::manageClient() {
	client = server.available();									//On met à jour les entrées
	if (!client.connected()) {
		Serial.println("Attente de connexions");
		delay(500);
		client = server.available();
		connected = client.connected();
		if (!connected && MotionControlSystem::Instance().isMoving() && millis()-lastMessage>1000) {			//Si ça fait trop longtemps qu'on a pas reçu de message
			Serial.println("No High Level, stop");
			MotionControlSystem::Instance().stop();
		}
	}
}

bool EthernetMgr::read(String& order, bool wait)
{
	//manageClient();
	client = server.available();
	if (wait) {
		while(!client){
			client = server.available();
		}
	}

	if (client) {							//Si on est connectés et il ya des choses à lire
		char readChar;
		char buffer[64]="";
		int i = 0;

		while (read_char(readChar) && i < RX_BUFFER_SIZE) {	//Tant qu'on n'est pas à la fin d'un message(\r)
			buffer[i]=readChar;
			i++;												//Au cas où on ne reçoit jamais de terminaison, on limite le nombre de chars
		}
		read_char(readChar);								//On élimine le \n
		lastMessage = millis();
		order.append(buffer);							//Conversion en string
		order = String(buffer);
		return (!order.equals(""));
	}
	else {
		return false;
	}

}

bool EthernetMgr::read(int16_t & value, bool wait)
{
	String readValue = "";

	bool status = read(readValue, wait);

	value = strtol(readValue.c_str(), nullptr, DEC);

	return status;
}

bool EthernetMgr::read(volatile int8_t & value, bool wait)
{
	String readValue = "";

	bool status = read(readValue, wait);

	value = strtol(readValue.c_str(), nullptr, DEC);

	return status;
}

bool EthernetMgr::read(float& value, bool wait) {
	String readValue = "";

	bool status = read(readValue, wait);

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
	Serial.print("Envoi:");
	Serial.println(logToSend);
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