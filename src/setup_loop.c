#include "comp/all.h"

#include "logging.h"
#include "my_hardware/timer.h"
#include "solution/calibrating.h"

void setup() {
}

#define LOOP_SIZE 20000000

void loop() {
	//calibrate_loop();
	float speed = 9.0f * (((float)(LOOP_SIZE - us_count() % LOOP_SIZE) / LOOP_SIZE) * 0.5 + 0.5);
	set_speed(motor_l, speed);
	set_speed(motor_r, speed);
	float enc_speed = get_speed(enc);
	logcsv("%f,%f", enc_speed, enc_speed - speed);
}
