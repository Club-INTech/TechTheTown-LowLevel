// defines.h

#ifndef _DEFINES_h
#define _DEFINES_h

#include <Arduino.h>
#include "define_debug.h"

/**
*COMMUNICATION
*/

// Nombre d'octets acceptables depuis le HL
constexpr uint8_t RX_BUFFER_SIZE = 64; // Taille max des messages
constexpr uint8_t RX_WORD_COUNT = 10; // Nombre de mots max par ordre

constexpr uint8_t NBR_OF_US_SENSOR = 4;

// Divers headers de communication pour l'éthernet
constexpr uint8_t HEADER_LENGTH = 2;

constexpr char STD_HEADER[2] = {0x40,0x40};
constexpr char DEBUG_HEADER[2] = {0x02,0x20};
constexpr char EVENT_HEADER[2] = {0x14,0x17};
constexpr char SENSOR_HEADER[2] = {0x01,0x06};
constexpr char POSITION_HEADER[2] = {0x07,0x05};


// Séparateurs des mots d'ordres
#define SEPARATOR  " "

// Fréquence d'envoi de la position
constexpr uint8_t F_ENV_POS = 50;


/**
*ASSERVISSEMENT
*/

constexpr uint16_t  MC_FREQUENCY =              1000; //1Khz
constexpr double    MC_PERIOD = static_cast<double>(1000000/MC_FREQUENCY); // Durée en µs entre deux mesures

constexpr uint16_t  TICKS_PER_TURN =            2400;   // Unité : ticks
constexpr float     COD_WHEEL_DIAMETER =        65.948;  // Unité : mm

constexpr uint8_t   DISTANCE_COD_GAUCHE_CENTRE = 112; // Unité : mm
constexpr uint8_t   DISTANCE_COD_DROITE_CENTRE = 111; // Unité : mm

constexpr float     TICK_TO_MM = static_cast<float>(PI*COD_WHEEL_DIAMETER/TICKS_PER_TURN); // Unité : mm/ticks
constexpr float     TICK_TO_RADIAN = static_cast<float>(TICK_TO_MM/DISTANCE_COD_GAUCHE_CENTRE); // Unité : rad/ticks

// Nombre de valeurs par moyenne
constexpr uint8_t   AVERAGE_SPEED_SIZE =        25;
constexpr uint8_t   AVERAGE_DERIVATIVE_SIZE =   100;
constexpr uint8_t   AVERAGE_US_SIZE =           10;

constexpr float     WHEEL_DISTANCE_TO_CENTER =  145.1;
constexpr uint8_t   TOLERANCY =                 50;

/**
*CAPTEURS
*/
constexpr uint8_t   CUBE_AV_DETECTION_ADDR = 0x10;
constexpr float     CUBE_AV_DETECTION_RANGE_MM = 50;
constexpr float     CUBE_AR_DETECTION_RANGE_MM = 50;

constexpr uint16_t BASIC_DETECTION_DISTANCE = 20;

#endif
