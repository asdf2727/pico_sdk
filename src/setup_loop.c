#include "comp/all.h"
#include "solution/task1.h"
#include "my_hardware/timer.h"

#include <hardware/regs/intctrl.h>
#include <hardware/structs/nvic.h>

void setup() {
}

uint32_t loop_len = 10000000;
uint32_t last_time = 0;

#include "logging.h"

void loop() {
	//task1_loop();

	uint32_t time = us_count();
	if (time - last_time >= 100000) {
		last_time = time;
		logcsv("%u,%u,%u,%u", get_spokes(enc), get_milimeters(front), ir_l, ir_r);
	}
}
