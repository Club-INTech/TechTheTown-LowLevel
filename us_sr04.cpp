#include "us_sr04.h"

SR04::SR04(uint8_t trigger_pin, uint8_t echo_pin) :pin_trigger(trigger_pin), pin_echo(echo_pin)
{
	pinMode(pin_trigger, OUTPUT);
	pinMode(pin_echo, INPUT);
	digitalWrite(pin_trigger, LOW);
	isInterruptOn = false;
}

uint16_t SR04::value()
{
	uint16_t valeurRetour = derniereDistance;
	derniereDistance = 0;
	return valeurRetour;
}

void SR04::setInterrupt(bool state)
{
	isInterruptOn = state;
}

void SR04::refresh()
{
	digitalWrite(pin_trigger, LOW);
	delayMicroseconds(2);
	//Impulsion de démarrage
	digitalWrite(pin_trigger, HIGH);
	delayMicroseconds(15);
	digitalWrite(pin_trigger, LOW);

	risingEdgeTrigger = true;
}

void SR04::interruption()
{
	if (risingEdgeTrigger) {
		//Premier passage: on est sur un front montant
		origineTimer = micros();
		risingEdgeTrigger = false;
		//On va maintenant attendre le front descendant
	}
	else {
		//On a reçu un front descendant:on peut calculer la durée
		uint32_t impulsion_duration, current_time;
		current_time = micros();
		impulsion_duration = current_time - origineTimer;
		ringBufferValeurs.append(10 * impulsion_duration / 58);
		derniereDistance = mediane(ringBufferValeurs);	//On renvoie la mediane du buffer, pour éviter les valeurs trop abérrantes
		isInterruptOn = false;							//On a eu ce qu'on voulait, on désactive l'interruption
	}
}