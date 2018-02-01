// defines.h

#ifndef _DEFINES_h
#define _DEFINES_h

#include <Arduino.h>

/**
*COMMUNICATION
*/

//Utilise la s�rie si true, l'ethernet si false
#define DEBUG true

//Nombre d'octets acceptables depuis le HL
#define RX_BUFFER_SIZE	64		//Taille max des messages
#define RX_WORD_COUNT	10		//Nombre de mots max par ordre

//Divers headers
#define HEADER_LENGTH   2

#define SENSOR_NB       4       //Nombre de valeurs de capteurs envoyées automatiquement

constexpr char STD_HEADER[2] = {0x40,0x40};
constexpr char DEBUG_HEADER[2] = {0x02,0x20};
constexpr char EVENT_HEADER[2] = {0x14,0x17};
constexpr char SENSOR_HEADER[2] = {0x01,0x06};
constexpr char POSITION_HEADER[2] = {0x07,0x05};

//Separateurs des mots d'ordres
#define SEPARATOR " "

//Frequence envoi position
#define F_ENV_POS 100


/**
*ASSERVISSEMENT
*/

#define MC_FREQUENCY 1000 //1kHz, � modifier
#define MC_PERIOD	(double)(1000000/MC_FREQUENCY)  //Dur�e en �s entre deux mesures

// 65,3mm diametre des roues
// ~1000 ticks par tour de roue
// 17cm écartement des roues

#define TICKS_PER_TURN 2400                //unité : ticks
#define COD_WHEEL_DIAMETER 65.3            //unité : mm

#define DISTANCE_COD_GAUCHE_CENTRE 112     //unité : mm
#define DISTANCE_COD_DROITE_CENTRE 111     //unité : mm
#define TICK_TO_MM (float)(PI*COD_WHEEL_DIAMETER/TICKS_PER_TURN)     // unité : mm/ticks old value 0.20567

//#define TICK_TO_RADIAN 0.0014569	// unité : radians/ticks
#define TICK_TO_RADIAN (float)(TICK_TO_MM/DISTANCE_COD_GAUCHE_CENTRE)

#define AVERAGE_SPEED_SIZE	25
#define AVERAGE_DERIVATIVE_SIZE 100

#define WHEEL_DISTANCE_TO_CENTER 145.1
#define TOLERANCY 50

#endif
