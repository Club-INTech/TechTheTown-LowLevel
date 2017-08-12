﻿#include "MotionControlSystem.h"

MotionControlSystem::MotionControlSystem() :leftEncoder(PIN_A_LEFT_ENCODER, PIN_B_LEFT_ENCODER),
											rightEncoder(PIN_A_RIGHT_ENCODER, PIN_B_RIGHT_ENCODER),
											leftMotor(Side::LEFT), rightMotor(Side::RIGHT), 
											serialHL(SerialHL::Instance()),
											rightSpeedPID(&currentRightSpeed, &rightPWM, &rightSpeedSetpoint),
											leftSpeedPID(&currentLeftSpeed, &leftPWM, &leftSpeedSetpoint),
											translationPID(&currentDistance, &translationSpeed, &translationSetpoint),
											rotationPID(&currentAngle, &rotationSpeed, &rotationSetpoint),
											averageLeftSpeed(), averageRightSpeed() {
	translationControlled = true;
	rotationControlled = true;
	leftSpeedControlled = true;
	rightSpeedControlled = true;
	controlled = true;
	originalAngle = 0.0;
	rotationSetpoint = 0;
	translationSetpoint = 0;
	leftSpeedSetpoint = 0;
	rightSpeedSetpoint = 0;

	lastLeftPWM = 0;
	lastRightPWM = 0;
	x = 0;
	y = 0;
	moving = false;
	moveAbnormal = false;
	forcedMovement = false;
	translation = true;
	direction = NONE;

	leftSpeedPID.setOutputLimits(-1023, 1023);
	rightSpeedPID.setOutputLimits(-1023, 1023);

	maxSpeed = 3000; // Vitesse maximum, des moteurs (avec une marge au cas o� on s'amuse � faire forcer un peu la bestiole).
	maxSpeedTranslation = 2000; // Consigne max envoy�e au PID
	maxSpeedRotation = 1400;


	maxAccelAv = 4;
	maxDecelAv = 8;
	maxAccelAr = 8;
	maxDecelAr = 4;

	delayToStop = 100; // temps � l'arr�t avant de consid�rer un blocage
	toleranceTranslation = 30;
	toleranceRotation = 50;
	toleranceSpeed = 40;
	toleranceSpeedEstablished = 50; // Doit �tre la plus petite possible, sans bloquer les trajectoires courbes 50
	delayToEstablish = 100;
	toleranceDifferentielle = 500; // Pour les trajectoires "normales", v�rifie que les roues ne font pas nawak chacunes de leur cot�.
	
	translationPID.setTunings(10, 0, 50);
	rotationPID.setTunings(17, 0, 220);
	leftSpeedPID.setTunings(0.011, 0, 0.005); // ki 0.00001
	rightSpeedPID.setTunings(0.011, 0, 0.005);

	maxAcceleration = maxAccelAr;
	maxDeceleration = maxDecelAr;

	leftMotor.init();
	rightMotor.init();
	enable(true);
}

void MotionControlSystem::control() {
	if (!controlled) {
		return;
	}
	// Pour le calcul de la vitesse instantanee :
	static int32_t previousLeftTicks = 0;
	static int32_t previousRightTicks = 0;

	// Pour le calcul de l'acceleration intantanee :
	static int32_t previousLeftSpeedSetpoint = 0;
	static int32_t previousRightSpeedSetpoint = 0;

	leftTicks = leftEncoder.read();
	rightTicks = rightEncoder.read();

	currentLeftSpeed = (leftTicks - previousLeftTicks) * 1000;
	currentRightSpeed = (rightTicks - previousRightTicks) * 1000;

	averageLeftSpeed.add(currentLeftSpeed);										//Mise à jour des moyennes des vitesses
	averageRightSpeed.add(currentRightSpeed);

	previousLeftTicks = leftTicks;
	previousRightTicks = rightTicks;

	averageLeftDerivativeError.add(ABS(leftSpeedPID.getDerivativeError()));		// Mise à jour des moyennes de dérivées de l'erreur (pour les blocages)
	averageRightDerivativeError.add(ABS(rightSpeedPID.getDerivativeError()));

	currentLeftSpeed = averageLeftSpeed.value(); // On utilise pour l'asserv la valeur moyenne des dernieres current Speed
	currentRightSpeed = averageRightSpeed.value(); // sinon le robot il fait nawak.

	currentDistance = (leftTicks + rightTicks) / 2;
	currentAngle = ((rightTicks - currentDistance)*RAYON_COD_GAUCHE / RAYON_COD_DROITE - (leftTicks - currentDistance)) / 2;

	//Mise à jour des pids
	if (translationControlled) {
		translationPID.compute();
	}
	if (rotationControlled) {
		rotationPID.compute();
	}
	
	// Limitation de la consigne de vitesse en translation
	if (translationSpeed > maxSpeedTranslation)
		translationSpeed = maxSpeedTranslation;
	else if (translationSpeed < -maxSpeedTranslation)
		translationSpeed = -maxSpeedTranslation;

	// Limitation de la consigne de vitesse en rotation
	if (rotationSpeed > maxSpeedRotation)
		rotationSpeed = maxSpeedRotation;
	else if (rotationSpeed < -maxSpeedRotation)
		rotationSpeed = -maxSpeedRotation;

	leftSpeedSetpoint = (int32_t)(translationSpeed - rotationSpeed);
	rightSpeedSetpoint = (int32_t)(translationSpeed + rotationSpeed);

	// Limitation de la vitesse
	if (leftSpeedSetpoint > maxSpeed) {
		leftSpeedSetpoint = maxSpeed;
	}
	else if (leftSpeedSetpoint < -maxSpeed) {
		leftSpeedSetpoint = -maxSpeed;
	}
	if (rightSpeedSetpoint > maxSpeed) {
		rightSpeedSetpoint = maxSpeed;
	}
	else if (rightSpeedSetpoint < -maxSpeed) {
		rightSpeedSetpoint = -maxSpeed;
	}
	
	//Limiteurs d'accélération et décélération
	if (leftSpeedSetpoint - previousLeftSpeedSetpoint > maxAcceleration)
	{
		leftSpeedSetpoint = (int32_t)(previousLeftSpeedSetpoint + maxAcceleration);
	}
	else if (previousLeftSpeedSetpoint - leftSpeedSetpoint > maxDeceleration) {
		leftSpeedSetpoint = (int32_t)(previousLeftSpeedSetpoint - maxDeceleration);
	}

	if (rightSpeedSetpoint - previousRightSpeedSetpoint > maxAcceleration) {
		rightSpeedSetpoint = (int32_t)(previousRightSpeedSetpoint + maxAcceleration);
	}
	else if (previousRightSpeedSetpoint - rightSpeedSetpoint > maxDeceleration) {
		rightSpeedSetpoint = (int32_t)(previousRightSpeedSetpoint - maxDeceleration );
	}

	previousLeftSpeedSetpoint = leftSpeedSetpoint;			// Mise à jour des consignes de vitesse pour calculs postérieurs
	previousRightSpeedSetpoint = rightSpeedSetpoint;

	//Mise à jour des PID en vitesse
	if (leftSpeedControlled) {
		leftSpeedPID.compute();
	}
	else {
		leftPWM = 0;
	}
	if (rightSpeedControlled) {
		rightSpeedPID.compute();
	}
	else {
		rightPWM = 0;
	}

	leftMotor.run(leftPWM);
	rightMotor.run(rightPWM);
}

MOVING_DIRECTION MotionControlSystem::getMovingDirection() const
{
	return direction;
}

void MotionControlSystem::enable(bool enable) {
	controlled = enable;
}

void MotionControlSystem::enableTranslationControl(bool enabled) {
	translationControlled = enabled;
}
void MotionControlSystem::enableRotationControl(bool enabled) {
	rotationControlled = enabled;
}

void MotionControlSystem::enableSpeedControl(bool enabled) {
	leftSpeedControlled = enabled;
	rightSpeedControlled = enabled;
}

bool MotionControlSystem::isPhysicallyStopped() {
	return ((translationPID.getDerivativeError() == 0) && (rotationPID.getDerivativeError() == 0)) || (ABS(ABS(leftSpeedPID.getError()) - ABS(rightSpeedPID.getError()))>toleranceDifferentielle);
}

bool MotionControlSystem::isLeftWheelSpeedAbnormal() {
	return (ABS(leftSpeedPID.getError())>toleranceSpeedEstablished);
}

bool MotionControlSystem::isRightWheelSpeedAbnormal() {
	return (ABS(rightSpeedPID.getError())>toleranceSpeedEstablished);
}

void MotionControlSystem::enableForcedMovement() {
	forcedMovement = true;
}

void MotionControlSystem::disableForcedMovement() {
	forcedMovement = false;
}


void MotionControlSystem::manageStop()
{
	static uint32_t time = 0;
	static uint32_t time2 = 0;
	static int32_t timeToEstablish = 0;
	static bool isSpeedEstablished = false;

	if (moving&&
		averageLeftDerivativeError.value()<toleranceSpeedEstablished &&
		averageRightDerivativeError.value()<toleranceSpeedEstablished &&

		leftSpeedPID.getError()<toleranceSpeed &&
		rightSpeedPID.getError()<toleranceSpeed &&

		!forcedMovement) {

		if (timeToEstablish == 0) {
			timeToEstablish = millis();
		}

		else if ((timeToEstablish > delayToEstablish) && !isSpeedEstablished) {
			isSpeedEstablished = true;

		}
	}

	if (isPhysicallyStopped() && moving && !forcedMovement) // Pour un blocage classique
	{
		if (time == 0)
		{ //Debut du timer
			time = millis();
		}
		else
		{
			if ((millis() - time) >= delayToStop)
			{ //Si arreté plus de 'delayToStop' ms
				{ //Stoppe pour cause de fin de mouvement
					moveAbnormal = !(ABS((translationPID.getError()) <= toleranceTranslation) && ABS(rotationPID.getError()) <= toleranceRotation);
					stop();
				}
			}
		}
	}
	else if (forcedMovement && moving) {

		if (time2 == 0)
		{
			time2 = millis();
		}
		else
		{
			if ((millis() - time2) >= delayToStop) {
				if (ABS(translationPID.getError()) <= toleranceTranslation && ABS(rotationPID.getError()) <= toleranceRotation)
				{ //Stoppé pour cause de fin de mouvement
					stop();
					moveAbnormal = false;

				}
			}
		}
	}



	else
	{
		time = 0;
		time2 = 0;
		if (moving)
			moveAbnormal = false;
	}
}


void MotionControlSystem::updatePosition() {
	static volatile int32_t lastDistance = 0;

	float deltaDistanceMm = (currentDistance - lastDistance) * TICK_TO_MM;
	lastDistance = currentDistance;

	x += (deltaDistanceMm * cosf(getAngleRadian()));
	y += (deltaDistanceMm * sinf(getAngleRadian()));
}

/**
* Ordres
*/


void MotionControlSystem::orderTranslation(int32_t mmDistance) {
	translationSetpoint += (int32_t)mmDistance / TICK_TO_MM;
	if (!moving)
	{
		translationPID.resetErrors();
		moving = true;
	}
	if (mmDistance >= 0) {
		direction = FORWARD;
		maxAcceleration = maxAccelAv;
		maxDeceleration = maxDecelAv;
	}
	else {
		direction = BACKWARD;
		maxAcceleration = maxAccelAr;
		maxDeceleration = maxDecelAr;
	}
	moveAbnormal = false;
}


void MotionControlSystem::orderRotation(float angleConsigneRadian, RotationWay rotationWay) {

	static int32_t deuxPiTick = (int32_t)(2 * PI / TICK_TO_RADIAN);
	static int32_t piTick = (int32_t)(PI / TICK_TO_RADIAN);

	int32_t highLevelOffset = originalAngle / TICK_TO_RADIAN;

	int32_t angleConsigneTick = angleConsigneRadian / TICK_TO_RADIAN;
	int32_t angleCourantTick = currentAngle + highLevelOffset;

	angleConsigneTick = modulo(angleConsigneTick, deuxPiTick);
	angleCourantTick = modulo(angleCourantTick, deuxPiTick);

	int32_t rotationTick = angleConsigneTick - angleCourantTick;

	if (rotationWay == FREE)
	{
		if (rotationTick >= piTick)
		{
			rotationTick -= deuxPiTick;
		}
		else if (rotationTick < -piTick)
		{
			rotationTick += deuxPiTick;
		}
	}
	else if (rotationWay == TRIGO)
	{
		if (rotationTick < 0)
		{
			rotationTick += deuxPiTick;
		}
	}
	else if (rotationWay == ANTITRIGO)
	{
		if (rotationTick > 0)
		{
			rotationTick -= deuxPiTick;
		}
	}

	rotationSetpoint = currentAngle + rotationTick;

	if (!moving)
	{
		rotationPID.resetErrors();
		moving = true;
	}
	direction = NONE;
	moveAbnormal = false;
}


void MotionControlSystem::orderRawPwm(Side side, int16_t pwm) {
	if (side == Side::LEFT)
		leftMotor.run(pwm);
	else
		rightMotor.run(pwm);
}


void MotionControlSystem::stop() {

	moving = false;
	translationSetpoint = currentDistance;
	rotationSetpoint = currentAngle;

	leftSpeedSetpoint = 0;
	rightSpeedSetpoint = 0;

	//Arrete les moteurs
	leftMotor.run(0);
	rightMotor.run(0);
	//Remet à zéro les erreurs
	translationPID.resetErrors();
	rotationPID.resetErrors();
	leftSpeedPID.resetErrors();
	rightSpeedPID.resetErrors();

	direction = NONE;
}

bool MotionControlSystem::isMoving() const
{
	return moving;
}

bool MotionControlSystem::isMoveAbnormal() const
{
	return moveAbnormal;
}


void MotionControlSystem::setRawPositiveTranslationSpeed() {
	translationSpeed = maxSpeedTranslation;
}

void MotionControlSystem::setRawPositiveRotationSpeed() {
	rotationSpeed = maxSpeedRotation;
}

void MotionControlSystem::setRawNegativeTranslationSpeed() {
	translationSpeed = -maxSpeedTranslation;
}

void MotionControlSystem::setRawNegativeRotationSpeed() {
	rotationSpeed = -maxSpeedRotation;
}

void MotionControlSystem::setRawNullSpeed() {
	rotationSpeed = 0;
	translationSpeed = 0;
}

/**
* Getters & Setters
*/

float MotionControlSystem::getAngleRadian() const
{
	return currentAngle;
}

void MotionControlSystem::setOriginalAngle(float newAngle)
{
	originalAngle = newAngle;
}

float MotionControlSystem::getX() const
{
	return x;
}

void MotionControlSystem::setX(float newX)
{
	x = newX;
}

float MotionControlSystem::getY() const
{
	return y;
}

void MotionControlSystem::setY(float newY)
{
	y = newY;
}

void MotionControlSystem::setTranslationSpeed(float raw_speed)
{
	// Conversion de raw_speed de mm / s en ticks / s
	double speed = raw_speed / TICK_TO_MM;

	if (speed < 0) { // SINGEPROOF
		maxSpeedTranslation = 0;
	}
	else {
		maxSpeedTranslation = (int32_t)speed;
	}
}

void MotionControlSystem::setRotationSpeed(float raw_speed)
{
	// Conversion de raw_speed de rad/s en ticks/s
	int speed = raw_speed / TICK_TO_RADIAN;

	if (speed < 0) {
		maxSpeedRotation = 0;
	}
	else {
		maxSpeedRotation = speed;
	}
}

void MotionControlSystem::getTranslationTunings(float &kp, float &ki, float &kd) const {
	kp = translationPID.getKp();
	ki = translationPID.getKi();
	kd = translationPID.getKd();
}
void MotionControlSystem::getRotationTunings(float &kp, float &ki, float &kd) const {
	kp = rotationPID.getKp();
	ki = rotationPID.getKi();
	kd = rotationPID.getKd();
}
void MotionControlSystem::getLeftSpeedTunings(float &kp, float &ki, float &kd) const {
	kp = leftSpeedPID.getKp();
	ki = leftSpeedPID.getKi();
	kd = leftSpeedPID.getKd();
}
void MotionControlSystem::getRightSpeedTunings(float &kp, float &ki, float &kd) const {
	kp = rightSpeedPID.getKp();
	ki = rightSpeedPID.getKi();
	kd = rightSpeedPID.getKd();
}
void MotionControlSystem::setTranslationTunings(float kp, float ki, float kd) {
	translationPID.setTunings(kp, ki, kd);
}
void MotionControlSystem::setRotationTunings(float kp, float ki, float kd) {
	rotationPID.setTunings(kp, ki, kd);
}
void MotionControlSystem::setLeftSpeedTunings(float kp, float ki, float kd) {
	leftSpeedPID.setTunings(kp, ki, kd);
}
void MotionControlSystem::setRightSpeedTunings(float kp, float ki, float kd) {
	rightSpeedPID.setTunings(kp, ki, kd);
}

void MotionControlSystem::setAccelAv() {
	serialHL.printfLog("entrer accel avant(là : %d )", this->maxAccelAv);
	serialHL.read(this->maxAccelAv);
	serialHL.printfLog("entrer decel avant(là : %d )", this->maxDecelAv);
	serialHL.read(this->maxDecelAv);
}

void MotionControlSystem::setAccelAr() {
	serialHL.printfLog("entrer accel arrière(là : %d )", this->maxAccelAr);
	serialHL.read(this->maxAccelAr);
	serialHL.printfLog("entrer decel arrière(là : %d )", this->maxDecelAr);
	serialHL.read(this->maxDecelAr);
}