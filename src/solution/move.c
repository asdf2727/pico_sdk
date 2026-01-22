#include "move.h"

#include <math.h>

#include "../comp/all.h"
#include "../my_hardware/timer.h"

#include "../logging.h"
#include "src/comp/infrared.h"
#include "src/comp/motor.h"

void move_straight(float percent) {
	float speed = MIN(get_max_speed(motor_l), get_max_speed(motor_r));
	speed *= percent * 0.999999f;
	logf("speed %f", speed);
	set_speed(motor_l, speed);
	set_speed(motor_r, speed);
}
void stop() {
	set_duty(motor_l, 0);
	set_duty(motor_r, 0);
}

inline float absf(float val) {
	return val > 0 ? val : -val;
}
void move_turn(float percent, float turn_speed) {
	float speed_l = 1 - WHEEL_SPACING * turn_speed;
	float speed_r = 1 + WHEEL_SPACING * turn_speed;
	float mult = MIN(get_max_speed(motor_l) / absf(speed_l),
	                 get_max_speed(motor_r) / absf(speed_r));
	mult *= percent * 0.999999f;
	logf("speeds %f %f", speed_l * mult, speed_r * mult);
	set_speed(motor_l, speed_l * mult);
	set_speed(motor_r, speed_r * mult);
}
void move_radius(float percent, float turn_rad) {
	float speed_l = turn_rad - WHEEL_SPACING;
	float speed_r = turn_rad + WHEEL_SPACING;
	float mult = MIN(get_max_speed(motor_l) / absf(speed_l),
	                 get_max_speed(motor_r) / absf(speed_r));
	mult *= percent * 0.999999f;
	set_speed(motor_l, speed_l * mult);
	set_speed(motor_r, speed_r * mult);
}

#define FOLLOW_MIN_T 10000

uint32_t last_follow = -1;
float last_offset;
float sum_offset = 0;

void move_follow(float percent) {
	if (~ir_status & 1) {
		last_follow = -1;
		sum_offset = 0;
		move_straight(percent);
		return;
	}
	if (last_follow == -1) {
		last_follow = us_count();
		last_offset = ir_offset;
		return;
	}
	uint32_t time = us_count();
	float delta_t = (time - last_follow) * 1e-6;
	if (time - last_follow < FOLLOW_MIN_T) return;
	last_follow = time;

	float step = (est_speed(motor_l) + est_speed(motor_r)) / 2 * delta_t;
	
	float P = -0.4 * ir_offset;
	float I = -0 * sum_offset;
	float D = -1 * (ir_offset - last_offset);// / delta_t;

	last_offset = ir_offset;
	sum_offset += ir_offset * delta_t;

	move_turn(percent, P+I+D);
	//logcsv("%f,%f,%f,%f", P,I,D, WHEEL_SPACING * (P+I+D));
}

#define CONVERGE_DIST 20

void move_follow_fancy(float percent) {
	if (~ir_status & 1) {
		last_follow = us_count();
		last_offset = ir_offset;
		return;
	}
	uint32_t time = us_count();
	float delta_t = (time - last_follow) * 1e-6;
	if (time - last_follow < FOLLOW_MIN_T) return;
	last_follow = time;

	float step = (est_speed(motor_l) + est_speed(motor_r)) / 2 * delta_t;
	float alpha = (est_speed(motor_r) - est_speed(motor_l)) / 2 / WHEEL_SPACING * delta_t;

	//	logf("step %f", step);
	
	float alpha_d = alpha * WHEEL_TO_SENSOR;
	float step_2 = step * step;
	float delta_offset = ir_offset - last_offset;
	last_offset = ir_offset;
	float a = alpha_d * alpha_d + step_2;
	float b = -(step_2 + 2 * alpha_d * delta_offset);
	float c = delta_offset * delta_offset;
	float D = b * b - 4 * a * c;
	if (D < 0) {
		//logf("Negative delta! %f", D);
		D = 0;
	}
	float cos0_2 = (-b + sqrtf(D)) / (2 * a);
	if (cos0_2 < 0 || cos0_2 > 1) {
		logf("Squared cos(theta0) out of bounds!!! %f", cos0_2);
		return;
	}
	float sign0 = signbit(delta_offset - alpha_d) == 0 ? 1 : -1;
	float cos0 = sqrtf(cos0_2);
	float sin0 = sign0 * sqrtf(1 - cos0_2);
	float theta0 = sign0 * acosf(cos0);
	float theta1 = theta0 + alpha;
	float y0 = last_offset / cos0 - WHEEL_TO_SENSOR * sin0;
	float y1 = y0 + step * sin0;
	float y2 = y1 + CONVERGE_DIST * sinf(theta1);
	if (absf(y2) > WHEEL_TO_SENSOR) {
		logf("Estimated position out of bounds!!! %f", y2);
		return;
	}
	float beta = -asinf(y2 / WHEEL_TO_SENSOR) - theta1;
	//logcsv("%f,%f", beta, ir_offset);
	move_turn(percent, beta / CONVERGE_DIST);
}
