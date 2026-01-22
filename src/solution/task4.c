#include "task4.h"

#include <math.h>

#include "move.h"
#include "src/comp/encoder.h"
#include "src/comp/infrared.h"
#include "src/comp/motor.h"
#include "state.h"

#include "../comp/all.h"
#include "../logging.h"

#define MAX_SPEED 0.9f
#define AVOID_RAD 20

int inter_dir;
float target_angle;
int avoid_armed;

void task4_loop() {
	uint32_t dist = get_milimeters(front);
	switch (state) {
		case FOLLOW:
			float speed = MAX_SPEED * (dist - 150) / 150;
			if (speed > MAX_SPEED) speed = MAX_SPEED;
			move_follow(speed);

			if (~ir_status & 1) {
				inter_dir = (ir_status & 2) ? -1 : 1;
				stop();
				state = INTER_ROT;
			}
			else if (dist < 150) {
				target_angle = get_angle(enc) - WHEEL_SPACING * M_PI_2 / WHEEL_RADIUS;
				stop();
				state = AVOID_TURN;
			}
			break;
		case AVOID_TURN:
			move_radius(-MAX_SPEED, 0);

			logf("%f,%f", get_angle(enc), target_angle);
			if (get_angle(enc) <= target_angle) {
				avoid_armed = 0;
				stop();
				state = AVOID_MOVE;
			}
			break;
		case AVOID_MOVE:
			move_radius(MAX_SPEED, AVOID_RAD);

			avoid_armed += (avoid_armed ^ ~ir_status) & 1;
			if (avoid_armed >= 3) {
				inter_dir = -1;
				state = INTER_ROT;
			}
			break;
		case INTER_ROT:
			move_radius(inter_dir * MAX_SPEED, 0);

			if (ir_status & 1) {
				stop();
				state = FOLLOW;
			}
			break;
		default:
			stop();
			break;
	}
}