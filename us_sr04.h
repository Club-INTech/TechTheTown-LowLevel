#ifndef _US_SR04_h
#define _US_SR04_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif
#define NB_VALEURS_MEDIANE_SRF  5

#include "ring_buffer.hpp"

typedef ring_buffer<uint32_t, NB_VALEURS_MEDIANE_SRF> ringBufferSRF;

/**
*  Classe pour capteurs HC-SR04 pour une utilisation simple(avec interruptions)
*  @author Ug (adaptation du travail de Sylvain & Thibaut ~MissFrance~)
*  @date Juin 2015
*/

//Données: Angle de mesure efficace: 15°, plage de mesure: 2cm à 400cm

class SR04
{
 protected:
	 uint8_t pin_trigger;
	 uint8_t pin_echo;

 public:

	 /**
	 * Principe de l'utilisation de ces capteurs US:
	 *  ---------------------------
	 *
	 *  La carte envoie une impulsion sur la pin TRIGGER pendant une duree de ~10µs pour demander une acquisition. 
	 *  Puis, apres une duree inconnue, le capteur envoie une impulsion sur la pin ECHO.
	 *   La duree de cette impulsion est proportionnelle a la distance entre les capteurs et l'objet detecte.
	 *
	 */

	 SR04(uint8_t trigger_pin, uint8_t echo_pin);
	 uint16_t value();
	 void setInterrupt(bool);
	 void interruption();
	 void refresh();
private:
	 ringBufferSRF ringBufferValeurs;
	 volatile uint32_t derniereDistance;		//contient la derniere distance acquise, prete a etre envoyee
	 volatile uint32_t origineTimer;			//origine de temps afin de mesurer une duree
	 bool risingEdgeTrigger;
	 bool isInterruptOn;
};

#endif

