#ifndef _PIN_MAPPING_h
#define _PIN_MAPPING_h

//TODO:pin mapping

/* Ethernet */
//Ne pas oublier: PIN13 (LED) utilisée par l'ethernet
#define PWD 15
#define RST 14

/* Serial RXTX */
	//AX
#define RX1 31
#define TX1 32

/* Codeuses */
	//Gauche
#define PIN_A_LEFT_ENCODER 20
#define PIN_B_LEFT_ENCODER 21
	//Droite
#define PIN_A_RIGHT_ENCODER 18
#define PIN_B_RIGHT_ENCODER 19

/* Moteurs */
	//Gauche
#define PIN_DIR_LEFT 5
#define PIN_PWM_LEFT 6
	//Droite
#define PIN_DIR_RIGHT 7
#define PIN_PWM_RIGHT 8

/* Electrovannes */
#define PIN_ELECTROVANNE_AV 2
#define PIN_ELECTROVANNE_AR 4

/* Pompe a vide */
#define PIN_PWM_POMPE 1

/* CAPTEURS */

	//US
#define PIN_TRIGGER_TEST 0
#define PIN_ECHO_TEST 0

	//CONT
#define PIN_JMPR 0
#define PIN_CONT1 0

#endif //_PIN_MAPPING_h
