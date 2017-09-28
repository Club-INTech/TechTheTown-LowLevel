// defines.h

#ifndef _DEFINES_h
#define _DEFINES_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif

/**
*COMMUNICATION
*/

//Utilise la série si true, l'ethernet si false
#define DEBUG true

//Nombre d'octets acceptables depuis le HL
#define RX_BUFFER_SIZE	64		//Taille max des messages
#define RX_WORD_SIZE		12		//Taille max des mots d'un ordre
#define RX_WORD_COUNT	4		//Nombre de mots max par ordre

//Divers headers
#define HEADER_LENGTH   2
#define DEBUG_HEADER    {0x02, 0x20}
#define EVENT_HEADER    {0x13, 0x37}
#define SENSOR_HEADER	{0x01, 0x10}

/**
*ASSERVISSEMENT
*/

#define MC_FREQUENCY 1000 //1kHz, à modifier
#define MC_PERIOD	(double)(1000000/MC_FREQUENCY)  //Durée en µs entre deux mesures

// 65,5mm diametre des roues
// ~1000 ticks par tour de roue
// 17cm ?cartement des roues

#define RAYON_COD_GAUCHE 140
#define RAYON_COD_DROITE 144
#define TICK_TO_MM 0.2088			// unit? : mm/ticks

//#define TICK_TO_RADIAN 0.0014569	// unit? : radians/ticks
#define TICK_TO_RADIAN (TICK_TO_MM/RAYON_COD_GAUCHE)

#define AVERAGE_SPEED_SIZE	25
#define AVERAGE_DERIVATIVE_SIZE 100

#define WHEEL_DISTANCE_TO_CENTER 145.1
#define TOLERANCY 50

#endif