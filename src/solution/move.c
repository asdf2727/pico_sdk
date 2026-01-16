#include "move.h"
#include "src/comp/all.h"
#include "src/comp/motor.h"

uint32_t last_time;
float last_speed_r;
float last_speed_l;

#define WHEEL_SPACING 5

void move_straight(float percent) {
	float speed = MIN(max_speed_l, max_speed_r) * percent;
	set_speed(motor_l, speed);
	set_speed(motor_l, speed);
}
int move_cm(float percent) {
	float speed = MIN(max_speed_l, max_speed_r) * percent;
	set_speed(motor_l, speed);
	set_speed(motor_l, speed);
}
void stop(float percent) {
	set_speed(motor_l, 0);
	set_speed(motor_l, 0);
}
void move_radius(float percent, float turn_rad) {
	float speed_l = (turn_rad - WHEEL_SPACING) / turn_rad;
	float speed_r = (turn_rad + WHEEL_SPACING) / turn_rad;
	float mult = MIN(max_speed_l / speed_l, max_speed_r / speed_r) * percent;
	speed_l *= mult;
	speed_r *= mult;
	set_speed(motor_l, speed_l);
	set_speed(motor_r, speed_r);
}
void move_turn(float percent, int dir) {
	float speed_l = -WHEEL_SPACING * dir;
	float speed_r = +WHEEL_SPACING * dir;
	float mult = MIN(max_speed_l / speed_l, max_speed_r / speed_r) * percent;
	speed_l *= mult;
	speed_r *= mult;
	set_speed(motor_l, speed_l);
	set_speed(motor_r, speed_r);
}
void move_line(float percent) {
	
}