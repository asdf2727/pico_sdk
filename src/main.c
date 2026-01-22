
#include "comp/all.h"
#include "my_hardware/timer.h"

#include "solution/calibrating.h"
#include "solution/move.h"
#include "solution/task1.h"
#include "solution/task2.h"
#include "solution/task3.h"
#include "solution/task4.h"
#include "solution/task5.h"
#include "solution/state.h"

#include "logging.h"
#include "src/comp/encoder.h"
#include "src/comp/motor.h"
#include <stdint.h>

uint32_t last_reset = 0;
uint32_t next_show = 0;

#define LOOP_SIZE 20000000

extern uint16_t ir_l, ir_r;
extern float norm_l, norm_r;

state_t state = FOLLOW;

void loop() {
	//calibrate_loop();
	task5_loop();
	if (us_count() > next_show) {
		float est_l = est_speed(motor_l);
		float est_r = est_speed(motor_r);
		float move_spd = (est_speed(motor_l) + est_speed(motor_r)) / 2;
		float turn_spd = (est_speed(motor_r) - est_speed(motor_l)) / 2 / WHEEL_SPACING;
		//logcsv("%f,%f,%f", norm_l, norm_r, ir_offset);
		//logcsv("%u,%f,%f,%u,%f,%f", ir_status, est_l, est_r, get_milimeters(front), get_angle(enc), ir_offset);
		next_show += 25000;
	}
}

void setup_components();
void update_components();

#include <pico/stdio.h>

int main() {
	stdio_init_all();
	setup_components();
	move_straight(0.6f);
	while (1) {
		update_components();
		loop();
	}
}
