#ifndef _PIN_MAPPING_h
#define _PIN_MAPPING_h

//TODO:pin mapping

/* Ethernet */
//Ne pas oublier: PIN13 (LED) utilisée par l'ethernet
#define PWD 15
#define RST 14

/* Serial RXTX */
//AX
#define RX1 0
#define TX1 1

/* Codeuses */
//Gauche
#define PIN_A_LEFT_ENCODER 22
#define PIN_B_LEFT_ENCODER 23

//Droite
#define PIN_A_RIGHT_ENCODER 20
#define PIN_B_RIGHT_ENCODER 21

/* Moteurs */
//Gauche
#define PIN_DIR_LEFT 2
#define PIN_PWM_LEFT 3
//Droite
#define PIN_DIR_RIGHT 4
#define PIN_PWM_RIGHT 5

/* Electrovannes */
#define PIN_ELECTROVANNE_AV 7
#define PIN_ELECTROVANNE_AR 8

/* Pompe a vide */
#define PIN_PWM_POMPE 9

/* CAPTEURS */

//US
#define PIN_TRIGGER_TEST 0
#define PIN_ECHO_TEST 0

//CONT
#define PIN_JMPR 0
#define PIN_CONT1 0

#endif //_PIN_MAPPING_h