#include "motor.h"

#include <math.h>
#include <stdlib.h>

#include "../my_hardware/pwm.h"
#include "../my_hardware/sio.h"


struct motor_t {
	int pinA;
	int pinB;
	float a, b, c;
};

motor_t *create_motor(int pinA, int pinB, float a, float b, float c) {
	PIN_SET_TYPE(pinA, PWM);
	pwm_set_freq(PWM_PIN_TO_SLICE(pinA), 1000);
	pwm_set_enabled(PWM_PIN_TO_SLICE(pinA), 1);
	PIN_SET_TYPE(pinB, SIO);
	SIO_SET_OUT(pinB);
	
	motor_t *motor = malloc(sizeof(motor_t));
	motor->pinA = pinA;
	motor->pinB = pinB;
	motor->a = a;
	motor->b = b;
	motor->c = c;
	return motor;
}

void delete_motor(motor_t *motor) {
	pwm_set_enabled(PWM_PIN_TO_SLICE(motor->pinA), 0);
	SIO_CLEAR(motor->pinB);
	free(motor);
}

void set_duty(motor_t *motor, float duty) {
	if (duty < 0) {
		duty += 1;
		SIO_SET(motor->pinB);
	}
	else {
		SIO_CLEAR(motor->pinB);
	}
	pwm_set_duty_cycle(motor->pinA, duty);
}

#include "../logging.h"

void set_speed(motor_t *motor, float speed) {
	float delta = motor->b * motor->b - 4 * motor->a * (motor->c - speed);
	if (delta < 0) {	
		log("Speed delta negative!!!");
		delta = 0;
	}
	float duty = (-motor->b + sqrtf(delta)) / (2 * motor->a);
	if (duty > 1) {
		log("Duty over 1!!!");
		duty = 1;
	}
	if (duty < -1) {
		log("Duty under -1!!!");
		duty = -1;
	}
	set_duty(motor, duty);
}
float est_speed(motor_t *motor) {
	float duty_cycle = pwm_get_duty_cycle(motor->pinA);
	return motor->a * duty_cycle * duty_cycle + motor->b * duty_cycle + motor->c;
}
inline int get_direction(motor_t *motor) {
	return 1 - 2 * SIO_OUT_GET(motor->pinB);
}