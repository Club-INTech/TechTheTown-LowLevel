// defines.h

#ifndef _DEFINES_h
#define _DEFINES_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include <Arduino.h>
#else
	#include <WProgram.h>
#endif

//Utilise la série si true, l'ethernet si false
#define DEBUG false

/*
ASSERVISSEMENT
*/

#define PERIOD_ASSERV 1000 //1kHz, à modifier

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

#define RX_BUFFER_SIZE 64


#if DEBUG
#define TRACKER_SIZE 		1500
#else
#define TRACKER_SIZE 		1
#endif

#endif

