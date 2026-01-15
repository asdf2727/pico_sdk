#include "task1.h"

#include "../comp/all.h"
#include "src/my_hardware/timer.h"

void task1_loop() {
	set_speed(motor_l, 0);
	set_speed(motor_r, 0);
	my_busy_wait_ms(1000);
	set_speed(motor_l, 0.6);
	set_speed(motor_r, 0.6);
	my_busy_wait_ms(3500);
	set_speed(motor_l, 1);
	set_speed(motor_r, 1);
	my_busy_wait_ms(3500);
}