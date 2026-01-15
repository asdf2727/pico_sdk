#include "comp/all.h"

#include "logging.h"
#include "src/my_hardware/timer.h"

void setup() {
}

#define LOOP_SIZE 5000000

void loop() {
	float speed = 4.0f * (LOOP_SIZE - us_count() % LOOP_SIZE) / LOOP_SIZE;
	set_speed(motor_l, speed);
	set_speed(motor_r, speed);
	logcsv("%f,%f,%f", get_speed(enc), est_speed(motor_l), est_speed(motor_r));
}
