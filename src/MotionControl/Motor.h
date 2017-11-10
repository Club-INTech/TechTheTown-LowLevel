/**
* Moteur.cpp
*
* Classe de gestion d'un moteur (PWM, direction...)
*
* Recapitulatif pins utilisees pour controler les deux moteurs :
*
* Gauche :
* 	-pins de sens : ??
* 	-pin de pwm : ??
* Droit :
* 	-pins de sens : ??
* 	-pin de pwm : ??
*
*/

#ifndef COMPLETE_LOW_LEVEL_MOTOR_H
#define COMPLETE_LOW_LEVEL_MOTOR_H

#include "Utils/safe_enum.hpp"
#include <stdint.h>
#include "Utils/pin_mapping.h"
#include "Utils/utils.h"
#include <Arduino.h>

struct direction_def {
	enum type {
		BACKWARD, FORWARD
	};
};
struct side_def {
	enum type {
		LEFT, RIGHT
	};
};

typedef safe_enum<direction_def> Direction;
typedef safe_enum<side_def> Side;

class Motor {
private:
	Side side;
	Direction direction;
	void setDirection(Direction);
	uint8_t pin_pwm;
	uint8_t pin_dir;
public:
	Motor(Side);
	int16_t pwm;
	void init();
	void run(int16_t);
};

#endif //COMPLETE_LOW_LEVEL_MOTOR_H