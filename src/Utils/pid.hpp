/**
 * pid.hpp
 *
 * Classe PID : impl�mente un r�gulateur PID (proportionnel int�gral d�riv�)
 *
 * Auteur : Paul BERNIER - bernier.pja@gmail.com
 */

#ifndef PID_HPP
#define PID_HPP

#include <stdint.h>
#include "utils.h"


class PID
{
public:


	PID(volatile int32_t* input, volatile int32_t* output, volatile int32_t* setPoint)
	{
		this->output = output;
		this->input = input;
		this->setPoint = setPoint;

		setOutputLimits(-2147483647, 2147483647);
		setTunings(0, 0, 0);
		epsilon = 0;
		pre_error = 0;
		derivative = 0;
		integral = 0;
		resetErrors();
	}

	void compute() {

		int32_t error = (*setPoint) - (*input);
		derivative = error - pre_error;
		integral += error;
		pre_error = error;

		int32_t result = (int32_t)(
				kp * error + ki * integral + kd * derivative);

		//Saturation
/*
		if (result > outMax) {
			result = outMax;
		} else if (result < outMin) {
			result = outMin;
		}*/



		//Seuillage de la commande
		if (ABS(result) < epsilon)
			result = 0;

		(*output) = result;
	}

	void setTunings(float kp, float ki, float kd) {
		if (kp < 0 || ki < 0 || kd < 0)
			return;

		this->kp = kp;
		this->ki = ki;
		this->kd = kd;
	}

	void setOutputLimits(int32_t min, int32_t max) {
		if (min >= max)
			return;

		outMin = min;
		outMax = max;

		if ((*output) > outMax)
			(*output) = outMax;
		else if ((*output) < outMin)
			(*output) = outMin;
	}

	int32_t getOutputLimit() const {
		return outMax;
	}

	void setEpsilon(int32_t seuil) {
		if(seuil < 0)
			return;
		epsilon = seuil;
	}

	int32_t getEpsilon() const {
		return epsilon;
	}

	void resetErrors() {
		pre_error = 0;
		integral = 0;
	}
	float getKp() const {
		return kp;
	}
	float getKi() const {
		return ki;
	}
	float getKd() const {
		return kd;
	}

	int32_t getError() const {
		return pre_error;
	}

	int32_t getDerivativeError() const {
		return derivative;
	}

	int32_t getIntegralErrol() const {
		return integral;
	}

	int getInput(){
		return (int)(*input);
	}
	int getOutput(){
		return (int)(*output);
	}
	int getSet(){
		return (int)(*setPoint);
	}

private:

	float kp;
	float ki;
	float kd;

	volatile int32_t* input; //Valeur du codeur
	volatile int32_t* output; //Output : pwm
	volatile int32_t* setPoint; //Valeur � atteindre

	int32_t epsilon;
	int32_t outMin, outMax;

	int32_t pre_error;
	int32_t derivative;
	int32_t integral;
};

#endif
