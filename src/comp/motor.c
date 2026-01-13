#include "motor.h"

#include <stdlib.h>

#include "../my_hardware/pwm.h"
#include "../my_hardware/sio.h"


struct motor_t {
	int pinA;
	int pinB;
};

motor_t *create_motor(int pinA, int pinB) {
	PIN_SET_TYPE(pinA, PWM);
	pwm_set_freq(PWM_PIN_TO_SLICE(pinA), 1000);
	pwm_set_enabled(PWM_PIN_TO_SLICE(pinA), 1);
	PIN_SET_TYPE(pinB, SIO);
	SIO_SET_OUT(pinB);
	
	motor_t *motor = malloc(sizeof(motor_t));
	motor->pinA = pinA;
	motor->pinB = pinB;
	return motor;
}

void delete_motor(motor_t *motor) {
	pwm_set_enabled(PWM_PIN_TO_SLICE(motor->pinA), 0);
	SIO_CLEAR(motor->pinB);
	free(motor);
}

void set_speed(motor_t *motor, float speed) {
	if (speed < 0) {
		speed += 1;
		SIO_SET(motor->pinB);
	}
	else {
		SIO_CLEAR(motor->pinB);
	}
	pwm_set_duty_cycle(motor->pinA, speed);
}
// TODO compute regression using encoder
float est_speed(motor_t *motor) {
	float duty_cycle = pwm_get_duty_cycle(motor->pinA);
	return 0 + 1 * duty_cycle + 0 * duty_cycle * duty_cycle;
}
inline int get_direction(motor_t *motor) {
	return 1 - 2 * SIO_OUT_GET(motor->pinB);
}