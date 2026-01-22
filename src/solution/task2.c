#include "task2.h"

#include "move.h"
#include "state.h"

#include "../comp/all.h"

#define MAX_SPEED 0.6f

void task2_loop() {
	uint32_t dist = get_milimeters(front);
	switch (state) {
		case FOLLOW:
			float speed = MAX_SPEED * (dist - 100) / 100;
			if (speed < MAX_SPEED) speed = MAX_SPEED;
			move_straight(speed);

			if (dist < 100) state = HALT;
			break;
		default:
			stop();
			break;
	}
}