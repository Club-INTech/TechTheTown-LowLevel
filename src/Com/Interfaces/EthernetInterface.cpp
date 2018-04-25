/**
*
*Classe gerant la communication avec le Haut Niveau(messages arrivant, sortant)
*
*ASCII:13 = CR, 10 = LF
*/

#include "EthernetInterface.h"

EthernetInterface::EthernetInterface():server{ PORT }
{
	resetCard();

	while(Ethernet.localIP() != ip) {
        digitalWrite(30,HIGH);
		pinMode(LED_BUILTIN,OUTPUT);
		digitalWrite(LED_BUILTIN,!digitalRead(LED_BUILTIN)); //La led de la teensy clignote si il y a erreur
		delay(200);
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

void EthernetInterface::resetCard() {

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
    Ethernet.init(PIN_SPI_SS);
    digitalWrite(30,LOW);
}

bool inline EthernetInterface::read_char(char & buffer)
{
    buffer = (char)client.read();
	return ((buffer != '\r' && buffer != '\n' ) && buffer != -1);
}

inline bool EthernetInterface::read(char* order)
{
	EthernetClient newClient = server.available();
	if (newClient.available()) {							//Si on est connectes et il ya des choses a lire
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

bool EthernetInterface::read(int32_t & value)
{
	char readValue[16];

	bool status = read(readValue);

	value = strtol(readValue, nullptr, DEC);

	return status;
}

bool EthernetInterface::read(int16_t & value)
{
	char readValue[16];

	bool status = read(readValue);

	value = strtol(readValue, nullptr, DEC);

	return status;
}

bool EthernetInterface::read(volatile int8_t & value)
{
	char readValue[16] = "";

	bool status = read(readValue);

	value = strtol(readValue, nullptr, DEC);

	return status;
}

bool EthernetInterface::read(float& value) {
	char readValue[16] = "";

	bool status = read(readValue);

	value = strtof(readValue, nullptr);

	return status;
}


void EthernetInterface::printf(const char *message) {
	client.print(message);
}

void EthernetInterface::printfln(const char* message) {
	client.println(message);
}