// defines.h

#ifndef _DEFINES_h
#define _DEFINES_h

#include <Arduino.h>
#include "define_debug.h"

/**
*COMMUNICATION
*/


// Nombre d'octets acceptables depuis le HL
#define RX_BUFFER_SIZE	64		// Taille max des messages
#define RX_WORD_COUNT	10		// Nombre de mots max par ordre


constexpr uint8_t NBR_OF_US_CAPTOR = 1;

// Divers headers de communication pour l'éthernet
#define HEADER_LENGTH   2

constexpr char STD_HEADER[2] = {0x40,0x40};
constexpr char DEBUG_HEADER[2] = {0x02,0x20};
constexpr char EVENT_HEADER[2] = {0x14,0x17};
constexpr char SENSOR_HEADER[2] = {0x01,0x06};
constexpr char POSITION_HEADER[2] = {0x07,0x05};

// Séparateurs des mots d'ordres
#define SEPARATOR " "

// Fréquence d'envoi de la position
#define F_ENV_POS 50


/**
*ASSERVISSEMENT
*/

#define MC_FREQUENCY 1000 // 1kHz
#define MC_PERIOD	(double)(1000000/MC_FREQUENCY)  // Durée en µs entre deux mesures

#define TICKS_PER_TURN 2400                // Unité : ticks
#define COD_WHEEL_DIAMETER 65.3            // Unité : mm

#define DISTANCE_COD_GAUCHE_CENTRE 112     // Unité : mm
#define DISTANCE_COD_DROITE_CENTRE 111     // Unité : mm

#define TICK_TO_MM (float)(PI*COD_WHEEL_DIAMETER/TICKS_PER_TURN)     // Unité : mm/ticks
#define TICK_TO_RADIAN (float)(TICK_TO_MM/DISTANCE_COD_GAUCHE_CENTRE)// Unité : radians/ticks

// Nombre de valeurs par moyenne
#define AVERAGE_SPEED_SIZE	25
#define AVERAGE_DERIVATIVE_SIZE 100

#define WHEEL_DISTANCE_TO_CENTER 145.1
#define TOLERANCY 50

#endif
