#include "task5.h"

#include "move.h"
#include "src/comp/encoder.h"
#include "src/comp/motor.h"
#include "state.h"

#include "../comp/all.h"

#define MAX_SPEED 0.85f
#define TRAVEL_DIST 180

void task5_loop() {
	uint32_t dist = get_angle(enc) * WHEEL_RADIUS;
	switch (state) {
		case FOLLOW:
			float speed = MAX_SPEED * (TRAVEL_DIST - dist) / 10;
			if (speed > MAX_SPEED) speed = MAX_SPEED;
			move_follow(speed);

			if (dist > TRAVEL_DIST) state = HALT;
			break;
		default:
			stop();
			break;
	}
}