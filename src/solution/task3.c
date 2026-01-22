#include "task3.h"

#include "move.h"
#include "state.h"

#include "../comp/all.h"

#define MAX_SPEED 0.9f

int mult;

void task3_loop() {
	switch (state) {
		case FOLLOW:
			move_follow(MAX_SPEED);

			if (~ir_status & 1) {
				mult = (ir_status & 2) ? -1 : 1;
				stop();
				state = INTER_ROT;
			}
			break;
		case INTER_ROT:
			move_radius(mult * MAX_SPEED, 0);
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