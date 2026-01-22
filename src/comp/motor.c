#include "motor.h"

#include <math.h>
#include <stdlib.h>

#include "../my_hardware/pwm.h"
#include "../my_hardware/sio.h"
#include "src/my_hardware/timer.h"

#include "../logging.h"

#define MAX_ACCEL 150 // in cm/s^2

struct motor_t {
	int pinA;
	int pinB;
	float a, b, c;
	float max_speed;
	uint32_t last_time;
	float last_duty;
	int last_dir;
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
	motor->max_speed = motor->a + motor->b + motor->c;
	motor->last_time = 0;
	motor->last_duty = 0;
	motor->last_dir = 0;
	return motor;
}

void delete_motor(motor_t *motor) {
	pwm_set_enabled(PWM_PIN_TO_SLICE(motor->pinA), 0);
	SIO_CLEAR(motor->pinB);
	free(motor);
}

void set_duty(motor_t *motor, float duty) {
	uint32_t time = us_count();
	float delta_duty = (time - motor->last_time) / (1000000 * motor->max_speed) * MAX_ACCEL;
	if (duty > motor->last_duty + delta_duty) {
		duty = motor->last_duty + delta_duty;
		//log("Accel limited!");
	}
	if (duty < motor->last_duty - delta_duty) {
		duty = motor->last_duty - delta_duty;
		//log("Accel limited!");
	}
	int new_dir = duty == 0 ? 0 : (signbit(duty) == 0 ? 1 : -1);
	if (motor->last_dir != new_dir) {
		motor->last_dir = new_dir;
		//if (new_dir == 0 || motor->last_dir == 0) duty = new_dir;
		duty = new_dir;
	}
	motor->last_duty = duty;
	motor->last_time = time;
	
	if (duty < 0) {
		duty += 1;
		SIO_SET(motor->pinB);
	}
	else {
		SIO_CLEAR(motor->pinB);
	}
	pwm_set_duty_cycle(motor->pinA, duty);
}

inline float absf(float val) {
	return val > 0 ? val : -val;
}
void set_speed(motor_t *motor, float speed) {
	int sign = 1;
	if (speed < 0) {
		speed = -speed;
		sign = -1;
	}
	float duty;
	if (absf(motor->a) < 0.01f) {
		duty = (speed - motor->c) / motor->b;
	}
	else {
		float delta = motor->b * motor->b - 4 * motor->a * (motor->c - speed);
		if (delta < 0) {
			log("Speed delta negative!!!");
			delta = 0;
		}
		duty = (-motor->b + sqrtf(delta)) / (2 * motor->a);
	}
	if (duty > 1) {
		//log("Duty over 1!!!");
		duty = 1;
	}
	if (duty < MIN_DUTY) duty = MIN_DUTY;
	set_duty(motor, sign * duty);
}
float est_speed(motor_t *motor) {
	float duty = motor->last_dir * motor->last_duty;
	float est = motor->a * duty * duty + motor->b * duty + motor->c;
	return est < 0 ? 0 : motor->last_dir * est;
}
inline int get_direction(motor_t *motor) {
	return motor->last_dir;
}
inline float get_max_speed(motor_t *motor) {
	return motor->max_speed;
}