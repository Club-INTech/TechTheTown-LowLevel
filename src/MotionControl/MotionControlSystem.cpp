﻿#include "MotionControlSystem.h"

MotionControlSystem::MotionControlSystem() :
											leftEncoder(Encoder(PIN_A_LEFT_ENCODER, PIN_B_LEFT_ENCODER)),
											rightEncoder(PIN_A_RIGHT_ENCODER, PIN_B_RIGHT_ENCODER),
											leftMotor(Side::LEFT), rightMotor(Side::RIGHT),
											rightSpeedPID(&currentRightSpeed, &rightPWM, &rightSpeedSetpoint),
											leftSpeedPID(&currentLeftSpeed, &leftPWM, &leftSpeedSetpoint),
											translationPID(&currentDistance, &translationSpeed, &translationSetpoint),
											rotationPID(&currentAngle, &rotationSpeed, &rotationSetpoint),
                                            maxAcceptableTranslationSpeed(10000), maxAcceptableRotationSpeed(6000),
											averageLeftSpeed(), averageRightSpeed(){
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
	moveAbnormalSent = false;
	forcedMovement = false;
    pointToPointMovement = false;
    sequentialPointToPoint = false;
    followTrajectory = false;
	translation = true;
	direction = NONE;

    trajectoryToFollow = new PointToPointTrajectory();

    wasMoving = false;

	leftSpeedPID.setOutputLimits(-255, 255);
	rightSpeedPID.setOutputLimits(-255, 255);


    maxSpeed = 12000;				// Limite globale de la vitesse (Rotation + Translation)
	maxSpeedTranslation = maxAcceptableTranslationSpeed;
	maxSpeedRotation = maxAcceptableRotationSpeed;


	delayToStop = 25;              // Temps a l'arret avant de considérer un blocage
	toleranceRadiale = 10;          // Rayon du cercle de tolérance du point à point avant de considérer une droite
	toleranceRadialeTrajectoire = 50;
    toleranceAngulairePtP = 0.25;
	toleranceTranslation = 50;
	toleranceRotation = 100;
	toleranceSpeed = 24;
	toleranceSpeedEstablished = 120; // Doit être la plus petite possible, sans bloquer les trajectoires courbes 50
	delayToEstablish = 100;
	toleranceDifferentielle = 2147483647;  // Pour les trajectoires "normales", verifie que les roues ne font pas nawak chacunes de leur cote.
	toleranceDerivative = 0; 		// Doit être suffisament faible pour être fiable mais suffisament élevée pour arrêter contre un mur


    leftSpeedPID.setTunings(0.2165,0.00005,0.414);
    rightSpeedPID.setTunings(0.225,0.00005,0.4121);

    translationPID.setTunings(6.5,0,1.08);
    rotationPID.setTunings(12,0.00001,0);

	maxAcceleration = 30;

	leftMotor.init();
	rightMotor.init();
	enable(true);
}

int32_t MotionControlSystem::getLeftTick()
{
	return leftTicks;
}

int32_t MotionControlSystem::getRightTick()
{
	return rightTicks;
}

void MotionControlSystem::updateTicks(){
    leftTicks = leftEncoder.read();
    rightTicks = rightEncoder.read();
}

void MotionControlSystem::control()
/**
 * LA fonction de l'asservissement
 * Contrôle les dépassements par rapport aux limites imposées (Vitesse, accélération)
 * Met à jour la position, l'angle et les PIDs
 * Donne le PWM aux moteurs
 */
{
	if (controlled) {

		// Pour le calcul de la vitesse instantanee :
		static int32_t previousLeftTicks = 0;
		static int32_t previousRightTicks = 0;

		// Pour le calcul de l'acceleration intantanee :
		static int32_t previousLeftSpeedSetpoint = 0;
		static int32_t previousRightSpeedSetpoint = 0;

		updateTicks();

		currentLeftSpeed = (leftTicks - previousLeftTicks) * MC_FREQUENCY;
		currentRightSpeed = (rightTicks - previousRightTicks) * MC_FREQUENCY;

		averageLeftSpeed.add(currentLeftSpeed);										//Mise à jour des moyennes des vitesses
		averageRightSpeed.add(currentRightSpeed);

		previousLeftTicks = leftTicks;
		previousRightTicks = rightTicks;

		averageLeftDerivativeError.add(ABS(leftSpeedPID.getDerivativeError()));		// Mise à jour des moyennes de dérivées de l'erreur (pour les blocages)
		averageRightDerivativeError.add(ABS(rightSpeedPID.getDerivativeError()));

		currentLeftSpeed = averageLeftSpeed.value();                                // On utilise pour l'asserv la valeur moyenne des dernieres current Speed
		currentRightSpeed = averageRightSpeed.value();                              // Sinon le robot il fait nawak.

		currentDistance = (leftTicks + rightTicks) / 2;
		currentAngle = ((rightTicks - currentDistance)*DISTANCE_COD_GAUCHE_CENTRE / DISTANCE_COD_DROITE_CENTRE - (leftTicks - currentDistance)) / 2;

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
		if (((leftSpeedSetpoint - previousLeftSpeedSetpoint)  > maxAcceleration) && moving)
		{
			leftSpeedSetpoint = (previousLeftSpeedSetpoint + maxAcceleration);
		}
		else if (((previousLeftSpeedSetpoint - leftSpeedSetpoint)  > maxAcceleration) && moving)
		{
			leftSpeedSetpoint = (previousLeftSpeedSetpoint - maxAcceleration);
		}
		if (((rightSpeedSetpoint - previousRightSpeedSetpoint)  > maxAcceleration) && moving)
		{
			rightSpeedSetpoint = (previousRightSpeedSetpoint + maxAcceleration);
		}
		else if (((previousRightSpeedSetpoint - rightSpeedSetpoint)  > maxAcceleration) && moving)
		{
			rightSpeedSetpoint = (previousRightSpeedSetpoint - maxAcceleration);
		}

		// Mise à jour des consignes de vitesse pour calculs postérieurs
		previousLeftSpeedSetpoint = leftSpeedSetpoint;
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
	bool translationBlockage = ABS(translationPID.getDerivativeError()) <= toleranceDerivative;
	bool rotationBlockage = ABS(rotationPID.getDerivativeError()) <= toleranceDerivative;
	bool speedDifferenceTooHigh = ABS(ABS(leftSpeedPID.getError()) - ABS(rightSpeedPID.getError()))>toleranceDifferentielle;
	/*Serial.print("Translation check: ");
	Serial.println(translationBlockage);
	Serial.print("Rotation check: ");
	Serial.println(rotationBlockage);
	Serial.print("Speed check: ");
	Serial.println(speedDifferenceTooHigh);*/
//	Serial.println("tests");
//	Serial.println(translationBlockage && rotationBlockage && moving);
//	Serial.println(speedDifferenceTooHigh && moving);
	return ((translationBlockage && rotationBlockage) || speedDifferenceTooHigh);
}

bool MotionControlSystem::isLeftWheelSpeedAbnormal() {
	return (ABS(leftSpeedPID.getError())>toleranceSpeedEstablished);
}

bool MotionControlSystem::isRightWheelSpeedAbnormal() {
	return (ABS(rightSpeedPID.getError())>toleranceSpeedEstablished);
}

void MotionControlSystem::enableForcedMovement(bool b) {
	forcedMovement = b;
}

void MotionControlSystem::disablePointToPoint()
{
	pointToPointMovement = false;
}

void MotionControlSystem::manageStop()
{
	static uint32_t time = 0;
	static uint32_t time2 = 0;

	if (isPhysicallyStopped() && moving && !forcedMovement) // Pour un blocage classique
	{
//		Serial.println("Je test le blocage");
		if (time == 0)
		{ //Debut du timer
			time = millis();
//			Serial.println("Je set le timer");
		}
		else
		{
//			Serial.println("Je vérifie si je suis bien arrêté");
//			Serial.print("dt: ");
//			Serial.println(millis()-time);
			if ((millis() - time) >= delayToStop)
			{ 	//Si arreté plus de 'delayToStop' ms
			  	// Stoppe pour cause de fin de mouvement
				moveAbnormal = !(ABS((translationPID.getError()) <= toleranceTranslation) && ABS(rotationPID.getError()) <= toleranceRotation);
				stop();
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
//		Serial.println("Je reset les timer");
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
    float currentAngleRadian = getAngleRadian();

    if(pointToPointMovement)
    {
        float moveVectorX = targetX - x;
        float moveVectorY = targetY - y;
        int moveNorm = (int)sqrtf(moveVectorX*moveVectorX+moveVectorY*moveVectorY);
        float moveArgument = atan2f(moveVectorY,moveVectorX);

		translationSetpoint = currentDistance;
		if(ABS(currentAngleRadian-moveArgument) > (float)PI)
		{
			if(moveArgument<0)
			{
				moveArgument += TWO_PI;
			}
			else
			{
				moveArgument -= TWO_PI;
			}
		}


		Serial.printf("X : %f\n",x);
		Serial.printf("Y : %f\n",y);
		Serial.printf("Angle actuel: %f\n",currentAngleRadian);
		Serial.printf("Déplacement en X: %f\n",moveVectorX);
		Serial.printf("Déplacement en Y: %f\n",moveVectorY);
		Serial.printf("Norme du déplacement : %i\n",moveNorm);
        Serial.printf("Angle du déplacement : %f\n",moveArgument);
		Serial.println("-----------------------");
		Serial.println(currentDistance);
		Serial.println(translationSetpoint);
		Serial.println(currentAngleRadian-moveArgument);
		Serial.println("-----------------------");
		Serial.println(currentAngleRadian-moveArgument>=(float)PI/2);
		Serial.println(ABS(currentAngleRadian-moveArgument)<(float)PI/2);
        Serial.println(currentAngleRadian-moveArgument<=(float)PI/2);
		Serial.println("~~~~~~~~~~~~~~~~~~~~~~~");


        if(currentAngleRadian-moveArgument>=(float)PI/2)
        {
            moveNorm = -moveNorm;
            moveArgument += (float)PI;
        }
        else if(currentAngleRadian-moveArgument<=-(float)PI/2)
        {
            moveNorm = -moveNorm;
            moveArgument -= (float)PI;
        }

        if(followTrajectory)
        {
            moveNorm *= 1-pow(currentAngleRadian-moveArgument,2)*(0.205148+0.0646868*pow(currentAngleRadian-moveArgument,2));
        }

        if(!sequentialPointToPoint || ( sequentialPointToPoint  && ABS(currentAngleRadian-moveArgument) <= toleranceAngulairePtP ))
        {
//            Serial.println("On avance ta mère");
            orderTranslation(moveNorm);
        }

        orderRotation(moveArgument,RotationWay::FREE);
        Serial.println(moveNorm);
        Serial.println(currentAngleRadian-moveArgument);

        if((ABS(moveNorm)<toleranceRadiale && !*trajectoryToFollow )||(followTrajectory && !*trajectoryToFollow))
        {
            Serial.println("On s'arrête pour tolérance");
            Serial.println(moveNorm);
            Serial.println(toleranceRadiale);

            forcedMovement = false;
            pointToPointMovement = false;
            followTrajectory = false;
            trajectoryToFollow = new PointToPointTrajectory();

            rotationSetpoint = currentAngle + (int32_t)(originalAngle/TICK_TO_RADIAN);
        }
        else if(ABS(moveNorm)<toleranceRadialeTrajectoire && *trajectoryToFollow)
        {
            Serial.println("#################");
            Serial.printf("Old target: %f - %f\n", targetX, targetY);

            std::pair<double,double> nextPoint = trajectoryToFollow->query();

            targetX += nextPoint.first;
            targetY += nextPoint.second;

            Serial.printf("New target: %f - %f\n", targetX, targetY);
            Serial.println("#################");
        }
		Serial.println("======================");
    }
}

void MotionControlSystem::resetPosition()
{
	this->setX(0);
	this->setY(0);
	this->setOriginalAngle(0);
	stop();
}

/**
* Ordres
*/


void MotionControlSystem::orderTranslation(int32_t mmDistance)
{
    /*if(mmDistance<300)
    {
        translationPID.setTunings(2.8,0,0);
    }
    else if(mmDistance<600)
    {
        translationPID.setTunings(0.4,0,0);
    }
    else
    {
        translationPID.setTunings(5.5,0,0);
    }*/
    
	translationSetpoint += (int32_t)mmDistance / TICK_TO_MM;
	if (!moving)
	{
		translationPID.resetErrors();
		moving = true;
        wasMoving = true;
	}

	direction = (mmDistance < 0) ? BACKWARD : FORWARD;

	moveAbnormal = false;
}


void MotionControlSystem::orderRotation(float targetAngleRadian, RotationWay rotationWay)
/**
 * Ordre de rotation
 * @param targetAngleRadian : Angle cible
 * @param rotationWay : Stratégie de rotation (FREE, TRIGO, ANTITRIGO)
 */
{
	static int32_t deuxPiTick = (int32_t)(2 * PI / TICK_TO_RADIAN);
	static int32_t piTick = (int32_t)(PI / TICK_TO_RADIAN);

	int32_t highLevelOffset = originalAngle / TICK_TO_RADIAN;

	int32_t targetAngleTick = targetAngleRadian / TICK_TO_RADIAN;
//	Serial.println(targetAngleTick);
	int32_t currentAngleTick = currentAngle + highLevelOffset;

    targetAngleTick = modulo(targetAngleTick, deuxPiTick);
	currentAngleTick = modulo(currentAngleTick, deuxPiTick);

	int32_t rotationTick = targetAngleTick - currentAngleTick;

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
        wasMoving = true;
	}
	direction = NONE;
	moveAbnormal = false;
}

void MotionControlSystem::orderGoto(float targetX, float targetY) {
    this->targetX = targetX;
    this->targetY = targetY;
    pointToPointMovement = true;
}

void MotionControlSystem::orderGoto(float targetX, float targetY, bool isSequential)
{
    this->orderGoto(targetX,targetY);
    sequentialPointToPoint = isSequential;
}

void MotionControlSystem::orderTrajectory(const double * xDeltas, const double *yDeltas, int length)
{
    forcedMovement = true;
    followTrajectory = true;
    trajectoryToFollow = new PointToPointTrajectory(xDeltas,yDeltas,length);
    std::pair<double,double> firstMove = trajectoryToFollow->query();
    orderGoto((float)(x + firstMove.first),(float)(y + firstMove.second));
}

void MotionControlSystem::orderRawPwm(Side side, int16_t pwm) {
	if (side == Side::LEFT)
		leftMotor.run(pwm);
	else
		rightMotor.run(pwm);
}

void MotionControlSystem::stop() {
	Serial.print(millis());
	Serial.println(" - STOPPING");
	Serial.println(currentDistance);
	Serial.println(currentAngle);

	moving = false;
    pointToPointMovement = false;
    followTrajectory = false;
    trajectoryToFollow = new PointToPointTrajectory();
	translationSetpoint = currentDistance;
	rotationSetpoint = currentAngle;

	leftSpeedSetpoint = 0;
	rightSpeedSetpoint = 0;
	translationSpeed = 0;
	rotationSpeed = 0;

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

bool MotionControlSystem::previousIsMoving() const
{
	return wasMoving;
}
void MotionControlSystem::setPreviousIsMoving(bool value)
{
	wasMoving=value;
}

bool MotionControlSystem::isMoveAbnormal() const
{
	return moveAbnormal;
}

bool MotionControlSystem::sentMoveAbnormal() const
{
	return moveAbnormalSent;
}

void MotionControlSystem::setMoveAbnormalSent(bool status) {
	moveAbnormalSent = status;
	translationSetpoint = currentDistance;
	rotationSetpoint = currentAngle;
	controlled = true;
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
	moving = false;
	rotationSpeed = 0;
	translationSpeed = 0;
}

/**
* Getters & Setters
*/

float MotionControlSystem::getAngleRadian() const
{
    float angleMod2 = modulo(currentAngle + (int32_t)(originalAngle/TICK_TO_RADIAN),(int32_t)(2 * PI / TICK_TO_RADIAN)) * TICK_TO_RADIAN;
    float angleFinal = angleMod2;
    if(angleMod2 > PI)
    {
        angleFinal = angleMod2 - (float)TWO_PI;
    }
    return(angleFinal);
}

void MotionControlSystem::setOriginalAngle(float newAngle)
{
	originalAngle = newAngle - (getAngleRadian() - originalAngle);
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
	float speed = raw_speed / TICK_TO_MM;

    if (speed < 0)
    { // SINGEPROOF
		maxSpeedTranslation = 0;
	}
    else if(speed>maxAcceptableTranslationSpeed)
    {
        maxSpeedTranslation = maxAcceptableTranslationSpeed;
    }
    else
    {
		maxSpeedTranslation = (int32_t)speed;
	}
}

void MotionControlSystem::setRotationSpeed(float raw_speed)
{
	// Conversion de raw_speed de rad/s en ticks/s
	float speed = raw_speed / TICK_TO_RADIAN;

    if (speed < 0)
    {
		maxSpeedRotation = 0;
	}
    else if(speed>maxAcceptableRotationSpeed)
    {
        maxSpeedRotation = maxAcceptableRotationSpeed;
    }
    else
    {
		maxSpeedRotation = (int32_t)speed;
	}
}

/* Getter des réglages du PID en translation */
void MotionControlSystem::getTranslationTunings(float &kp, float &ki, float &kd) const {
	kp = translationPID.getKp();
	ki = translationPID.getKi();
	kd = translationPID.getKd();
}
void MotionControlSystem::getTranslationErrors(int32_t& translationProp, int32_t& translationIntegral, int32_t& translationDerivative) {
    translationProp = translationPID.getError();
    translationIntegral = translationPID.getIntegralErrol();
    translationDerivative = translationPID.getDerivativeError();
}

/* Getter des réglages de PID en rotation */
void MotionControlSystem::getRotationTunings(float &kp, float &ki, float &kd) const {
	kp = rotationPID.getKp();
	ki = rotationPID.getKi();
	kd = rotationPID.getKd();
}
void MotionControlSystem::getRotationErrors(float& rotaProp, float& rotaIntegral, float& rotaDerivative) {
    rotaProp = rotationPID.getError()*TICK_TO_RADIAN;
    rotaIntegral = rotationPID.getIntegralErrol()*TICK_TO_RADIAN;
    rotaDerivative = rotationPID.getDerivativeError()*TICK_TO_RADIAN;
}

/* Getters des réglages de PID de vitesse */
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

/* Getters de vitesse */
float MotionControlSystem::getLeftSpeed() {
    return(currentLeftSpeed*TICK_TO_MM);
}
float MotionControlSystem::getRightSpeed() {
    return(currentRightSpeed*TICK_TO_MM);
}
int32_t MotionControlSystem::getDistanceTicks() {
    return(currentDistance);
}

/* Setters des réglages de PID */
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

void MotionControlSystem::getPWMS(int32_t& left, int32_t& right) {
	left = leftPWM;
	right = rightPWM;
}

void MotionControlSystem::getSpeedErrors(float& leftProp, float& leftIntegral, float& leftDerivative, float& rightProp, float& rightIntegral, float& rightDerivative) {
	leftProp = leftSpeedPID.getError();
	leftIntegral = leftSpeedPID.getIntegralErrol();
	leftDerivative = leftSpeedPID.getDerivativeError();

	rightProp = rightSpeedPID.getError();
	rightIntegral = rightSpeedPID.getIntegralErrol();
	rightDerivative = rightSpeedPID.getDerivativeError();
}

void MotionControlSystem::rawWheelSpeed(uint16_t speed, int32_t& leftOut,int32_t& rightOut)
/**
 * Envoie une vitesse brute aux moteurs
 * @param speed : Vitesse cible
 * @param leftOut : Variable pour récupérer la vitesse actuelle (Gauche)
 * @param rightOut : Variable pour récupérer la vitesse actuelle (Droite)
 */
{
	controlled = false;
	translationControlled = false;
	rotationControlled = false;

	leftSpeedSetpoint = speed;
	rightSpeedSetpoint = speed;
	while (ABS(currentLeftSpeed - leftSpeedSetpoint) > 100 && ABS(currentRightSpeed - rightSpeedSetpoint) > 100) {
		delayMicroseconds(MC_FREQUENCY);


		static int32_t previousLeftTicks = 0;
		static int32_t previousRightTicks = 0;

		leftTicks = leftEncoder.read();
		rightTicks = rightEncoder.read();

		currentLeftSpeed = (leftTicks - previousLeftTicks) * MC_FREQUENCY;
		currentRightSpeed = (rightTicks - previousRightTicks) * MC_FREQUENCY;

		averageLeftSpeed.add(currentLeftSpeed);										// Mise à jour des moyennes des vitesses
		averageRightSpeed.add(currentRightSpeed);

		previousLeftTicks = leftTicks;
		previousRightTicks = rightTicks;

		averageLeftDerivativeError.add(ABS(leftSpeedPID.getDerivativeError()));		// Mise à jour des moyennes de dérivées de l'erreur (pour les blocages)
		averageRightDerivativeError.add(ABS(rightSpeedPID.getDerivativeError()));

		currentLeftSpeed = averageLeftSpeed.value();                                // On utilise pour l'asserv la valeur moyenne des dernieres current Speed
		currentRightSpeed = averageRightSpeed.value();                              // Sinon le robot il fait nawak.


		leftSpeedPID.compute();
		rightSpeedPID.compute();

		leftMotor.run(leftPWM);
		rightMotor.run(rightPWM);
	}

	leftOut = currentLeftSpeed;
	rightOut = currentRightSpeed;

	translationControlled = true;
	rotationControlled = true;
	controlled = true;
}

void MotionControlSystem::getSpeedSetpoints(int32_t& left, int32_t& right) {
	left = leftSpeedSetpoint*TICK_TO_MM;
	right = rightSpeedSetpoint*TICK_TO_MM;
}

void MotionControlSystem::printValues() {

	Serial.print("LeftAvg  - ");Serial.println(this->averageLeftSpeed.value());
	Serial.print("RightAvg - "); Serial.println(this->averageRightSpeed.value());
	Serial.print("LeftPwm  - "); Serial.println(this->leftPWM);
	Serial.print("RightPwm - "); Serial.println(this->rightPWM);
}

void MotionControlSystem::resetPIDErrors(){
	leftSpeedPID.resetErrors();
	rightSpeedPID.resetErrors();
	translationPID.resetErrors();
	rotationPID.resetErrors();
}
