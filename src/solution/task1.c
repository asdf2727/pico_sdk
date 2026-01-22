#include "task1.h"

#include "../comp/all.h"
#include "src/my_hardware/timer.h"
#include "move.h"

extern uint32_t last_reset;

#define STEP_SIZE 3500000

void task1_loop() {
	uint32_t time = us_count() - last_reset;
	if (time < 1000000) {
		stop();
		return;
	}
	time -= 1000000;
	if (time < STEP_SIZE) {
		move_straight(0.5);
		return;
	}
	time -= STEP_SIZE;
	if (time < STEP_SIZE) {
		move_straight(1);
		return;
	}
	time -= STEP_SIZE;
	if (time < STEP_SIZE) {
		move_straight((float)(STEP_SIZE - time) / STEP_SIZE);
		return;
	}
	time -= STEP_SIZE;
	last_reset += 1000000 + 3 * STEP_SIZE;
}