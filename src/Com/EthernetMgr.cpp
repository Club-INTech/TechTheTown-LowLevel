/**
*
*Classe gerant la communication avec le Haut Niveau(messages arrivant, sortant)
*
*ASCII:13 = CR, 10 = LF
*/

#include "EthernetMgr.h"

EthernetMgr::EthernetMgr():server{ PORT }
{
	resetCard();

	while(Ethernet.localIP() != ip) {
		Serial.println("ERR\tIP CONFIGURATON FAILED");
        resetCard();
	}

    Serial.print("Ethernet Ready\nLocal ip: ");
    Serial.println(Ethernet.localIP());
    server.begin();
    delay(1000);
    client = server.available();
    if (client.connected()) {
        client.println("CONNECTED");
    }
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

inline bool EthernetMgr::read(char* order)
{
	EthernetClient newClient = server.available();
	if (newClient) {							//Si on est connectes et il ya des choses a lire
		client=newClient;
		char readChar;
		int i = 0;

		while (read_char(readChar) && i < RX_BUFFER_SIZE) {	//Tant qu'on n'est pas a la fin d'un message(\r)
			order[i] = readChar;
			i++;												//Au cas ou on ne recoit jamais de terminaison, on limite le nombre de chars
		}
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


void EthernetMgr::printf(const char *message, ...) {
    va_list args;								//Variable contenant la liste des arguments apr�s log (...)
    va_start(args, message);

    client.printf(message,args);

    va_end(args);
}

void EthernetMgr::printfln(const char* message, ...) {
	char data[HEADER_LENGTH + 64] = STD_HEADER;
	data[HEADER_LENGTH] = '\0';

	strcat(data, message);

    va_list args;
    va_start(args, message);

    client.printf(message,va_arg(args,String));
    client.println();

    va_end(args);
}

/**
*	Envoie une chaine de caracteres commencant par 2 headers Ultrason, puis les SENSOR_NB valeurs separees par des espaces
*/
void EthernetMgr::sendUS(uint16_t values[])
{
    String valueString="";
    char header[HEADER_LENGTH]=SENSOR_HEADER;
    valueString.append(header[0]);
    valueString.append(header[1]);
    for(int i=0;i<SENSOR_NB;i++){
        valueString.append(values[i]);
        valueString.append(" ");
    }

	//printfln(valueString.c_str());
	client.println(valueString);
}

/**
*	Envoie une chaine de caracteres commencant par 2 headers event, puis le nom de l'event
*/
void EthernetMgr::sendEvent(const char* event)
{
	String valueString="";
	char header[HEADER_LENGTH]=EVENT_HEADER;
	valueString.append(header[0]);
	valueString.append(header[1]);
	valueString.append(event);
	println(valueString);
}


/**
*	Envoie une chaine de caracteres commencant par 2 headers position, puis les valeurs x, y, angle séparées par des espaces
*/
void EthernetMgr::sendPosition(const float* pos)
{
	String valueString="";
	char header[HEADER_LENGTH]=POSITION_HEADER;
	valueString.append(header[0]);
	valueString.append(header[1]);
	for(int i=0;i<3;i++){
		valueString.append(pos[i]);
		valueString.append(" ");
	}
	println(valueString);
}


/**
*	Envoie une chaine de caracteres commencant par 2 headers debug, puis le message de log
*/
void EthernetMgr::log(const char* logs, ...) {
	char data[HEADER_LENGTH + 64] = DEBUG_HEADER;
	data[HEADER_LENGTH] = '\0';

	strcat(data, logs);

	va_list args;
	va_start(args, logs);

	client.printf(data,va_arg(args,String));
    client.println();

	va_end(args);
}