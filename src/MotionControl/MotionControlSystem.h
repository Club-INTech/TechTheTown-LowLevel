#ifndef MOTION_CONTROL_H_
#define MOTION_CONTROL_H_


#include "Utils/Singleton.hpp"
#include "Motor.h"
#include "Utils/pid.hpp"
#include "Utils/average.hpp"
#include "Utils/pin_mapping.h"
#include "Utils/defines.h"
#include "MotionControl/Encoder.h"

enum MOVING_DIRECTION { FORWARD, BACKWARD, NONE };

class MotionControlSystem : public Singleton<MotionControlSystem>
{
public:
	enum RotationWay {
		FREE, //Sens correspondant au chemin le plus court 
		TRIGO,//Sens Trigo(vers la gauche
		ANTITRIGO// Bah c'est dans l'autre sens
	};

	volatile bool translation;


private:

	//Codeuses
	Encoder leftEncoder;
	Encoder rightEncoder;
	int32_t leftTicks;
	int32_t rightTicks;

	//Moteurs
	Motor leftMotor;
	Motor rightMotor;

	/*
	* 		Définition des variables d'état du système (position, vitesse, consigne, ...)
	*
	* 		Les unités sont :
	* 			Pour les distances		: ticks
	* 			Pour les vitesses		: ticks/seconde
	* 			Pour les accélérations	: ticks/seconde^2
	* 			Ces unités seront vraies pour une fréquence d'asservissement de 2kHz,
	* 			si l'on souhaite changer la fréquence d'asservissement il faut adapter le calcul de la vitesse
	* 			autrement les unités ci-dessus ne seront plus valables.
	*/

	//	Asservissement en vitesse du moteur droit
	PID rightSpeedPID;
	volatile int32_t rightSpeedSetpoint;    // ticks/seconde
	volatile int32_t currentRightSpeed;        // ticks/seconde
	volatile int32_t rightPWM;
	volatile int32_t lastRightPWM;

	//	Asservissement en vitesse du moteur gauche
	PID leftSpeedPID;
	volatile int32_t leftSpeedSetpoint;        // ticks/seconde
	volatile int32_t currentLeftSpeed;        // ticks/seconde
	volatile int32_t leftPWM;
	volatile int32_t lastLeftPWM;

	//	Asservissement en position : translation
	PID translationPID;
	volatile int32_t translationSetpoint;    // ticks
	volatile int32_t currentDistance;        // ticks
	volatile int32_t translationSpeed;        // ticks/seconde

											  //	Asservissement en position : rotation
	PID rotationPID;
	volatile int32_t rotationSetpoint;        // angle absolu vis� (en ticks)
	volatile int32_t currentAngle;            // ticks
	volatile int32_t rotationSpeed;            // ticks/seconde

											   //	Limitation de vitesses
	volatile int32_t maxSpeed;                // definit la vitesse maximal des moteurs du robot
	volatile int32_t maxSpeedTranslation;    // definit la consigne max de vitesse de translation envoi�e au PID (trap�ze)
	volatile int32_t maxSpeedRotation;        // definit la consigne max de vitesse de rotation envoi�e au PID (trap�ze)

											  //	Limitation d'acc�l�ration
	volatile int8_t maxAcceleration;
	volatile int8_t maxDeceleration;


	//	Pour faire de jolies courbes de r�ponse du syst�me, la vitesse moyenne c'est mieux !
	Average<int32_t, AVERAGE_SPEED_SIZE> averageLeftSpeed;
	Average<int32_t, AVERAGE_SPEED_SIZE> averageRightSpeed;

	//Moyennes de la d�riv�e des erreurs (pour detecter blocages)
	Average<int32_t, AVERAGE_DERIVATIVE_SIZE> averageLeftDerivativeError;
	Average<int32_t, AVERAGE_DERIVATIVE_SIZE> averageRightDerivativeError;
	


	/*
	* 	Variables de positionnement haut niveau (exprimm�es en unit�s pratiques ^^)
	*
	*	Pingu <3
	*
	* 	Toutes ces variables sont initialis�es � 0. Elles doivent donc �tre r�gl�es ensuite
	* 	par le haut niveau pour correspondre � son syst�me de coordonn�es.
	* 	Le bas niveau met � jour la valeur de ces variables et les
	*/
	volatile float x;                // Positionnement 'x' (mm)
	volatile float y;                // Positionnement 'y' (mm)
	volatile float originalAngle;    // Angle d'origine	  (radians)
									 // 'originalAngle' repr�sente un offset ajout� � l'angle courant pour que nos angles en radians co�ncident avec la repr�sentation haut niveau des angles.

									 // Variables d'�tat du mouvement
	volatile bool moving;
	volatile MOVING_DIRECTION direction;
	volatile bool moveAbnormal;
	volatile bool moveAbnormalSent;

	// Variables d'activation des diff�rents PID
	volatile bool translationControlled;
	volatile bool rotationControlled;
	volatile bool leftSpeedControlled;
	volatile bool rightSpeedControlled;

	volatile bool forcedMovement; // Si true, alors pas de gestion de l'arret : ON FORCE MODAFUCKA !!!

								  // Variables de r�glage de la d�tection de blocage physique
	unsigned int delayToStop;  //En ms

	//Nombre de ticks de tol�rance pour consid�rer qu'on est arriv� � destination
	int toleranceTranslation;
	int toleranceRotation;

	int toleranceSpeed; // Tol�rance avant de consid�rer le mouvement anormal (�cart entre la consigne de vitesse et la vitesse r�elle)
	int toleranceSpeedEstablished; // Tol�rance autour de la vitesse �tablie avant de capter un blocage

	int toleranceDifferentielle;

	int delayToEstablish; // Temps � attendre avant de consid�rer la vitesse stable

	bool speedTest;

	bool isPhysicallyStopped();//Indique si le robot est immobile.
	bool isLeftWheelSpeedAbnormal();
	bool isRightWheelSpeedAbnormal();



public:
	MotionControlSystem();

	/* Codeuses */
	int32_t getLeftTick();
	int32_t getRightTick();

	/* Asservissement */
		//LA fonction d'asservissement
	void control();
	bool controlled;
		//Vitesse

	void setLeftSpeedTunings(float, float, float);
	void setRightSpeedTunings(float, float, float);
	void getPWMS(uint16_t & left, uint16_t & right);
	void getSpeedErrors(float & leftProp, float & leftIntegral, float & leftDerivative, float & rightProp, float & rightIntegral, float & rightDerivative);
	void rawWheelSpeed(uint16_t speed, uint16_t& leftsetpoint, uint16_t& rightsetpoint);
	void getSpeedSetpoints(uint16_t & left, uint16_t & right);
	void printValues();
	void getTranslationTunings(float &, float &, float &) const;
    void getTranslationErrors(float& translationProp, float& translationIntegral, float& translationDerivative);
	void getRotationTunings(float &, float &, float &) const;
    void getRotationErrors(float& rotaProp, float& rotaIntegral, float& rotaDerivative);

		//Position & orientation


	void setTranslationTunings(float, float, float);
	void setRotationTunings(float, float, float);
	void getLeftSpeedTunings(float &, float &, float &) const;
	void getRightSpeedTunings(float &, float &, float &) const;
    float getLeftSpeed();
    float getRightSpeed();

	void setRawPositiveTranslationSpeed();

	void setRawPositiveRotationSpeed();

	void setRawNegativeTranslationSpeed();

	void setRawNegativeRotationSpeed();

	void setRawNullSpeed();

	float getAngleRadian() const;
	void setOriginalAngle(float);
	float getX() const;
	void setX(float);
	float getY() const;
	void setY(float);

	void setTranslationSpeed(float);
	void setRotationSpeed(float);

	void updatePosition();
	void resetPosition(void);

	/* Ordres */
	void orderTranslation(int32_t);
	void orderRotation(float, RotationWay);
	void orderRawPwm(Side, int16_t);

	/* Autres */
	void stop();
	bool isMoving() const;
	bool isMoveAbnormal() const;
	bool sentMoveAbnormal() const;
	void setMoveAbnormalSent(bool);

	MOVING_DIRECTION getMovingDirection() const;
	void enable(bool);
	void enableTranslationControl(bool);
	void enableRotationControl(bool);
	void enableSpeedControl(bool);
	void manageStop();
	void enableForcedMovement();
	void disableForcedMovement();
};

#endif