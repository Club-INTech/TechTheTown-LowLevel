#include "Motor.h"

void Motor::setDirection(Direction directionToSet)
{
	direction = directionToSet;
	if (side == Side::LEFT) {
		if (direction == Direction::FORWARD) {
			digitalWrite(pin_dir, LOW);
		}
		if (direction == Direction::BACKWARD) {
			digitalWrite(pin_dir, HIGH);
		}
	}
	else {
		if (direction == Direction::FORWARD) {
			digitalWrite(pin_dir, HIGH);
		}
		if (direction == Direction::BACKWARD) {
			digitalWrite(pin_dir, LOW);
		}
	}
}

Motor::Motor(Side definedSide):side(definedSide), direction(Direction::FORWARD)
{
	pwm = 0;
}

//Initialise le spins, le pwm, bref tout ce dont le moteur a besoin
void Motor::init()
{
	if (side == Side::LEFT) {
		pin_pwm = PIN_PWM_LEFT;
		pin_dir = PIN_DIR_LEFT;
		pinMode(PIN_DIR_LEFT, OUTPUT);
		pinMode(PIN_PWM_LEFT, OUTPUT);
	}
	else if (side == Side::RIGHT) {
		pin_pwm = PIN_PWM_RIGHT;
		pin_dir = PIN_DIR_RIGHT;
		pinMode(PIN_DIR_RIGHT, OUTPUT);
		pinMode(PIN_PWM_RIGHT, OUTPUT);
	}

	//TODO: Initialiser les PWM
	analogWriteResolution(8);
	analogWriteFrequency(pin_pwm, 35156.25);
}

void Motor::run(int16_t newpwm)
{
	pwm = newpwm;
	if (pwm >= 0) {
		setDirection(Direction::FORWARD);
		pwm = MIN(pwm, 255);
	}
	else if (pwm < 0) {
		setDirection(Direction::BACKWARD);
		pwm = MIN(-pwm, 255);
	}
	analogWrite(pin_pwm, pwm);
}