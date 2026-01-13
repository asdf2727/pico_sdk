#include "my_hardware/timer.h"
#include "my_hardware/sio.h"
#include "comp/encoder.h"
#include "comp/motor.h"

#include "logging.h"
#include "src/comp/motor.h"
#include "src/comp/ultrasonic.h"
#include <hardware/regs/intctrl.h>
#include <hardware/structs/nvic.h>

#define PIN_LED 25

motor_t *motor_l;
motor_t *motor_r;

encoder_t *enc; // On L motor

ultrasonic_t *front;
ultrasonic_t *side;

extern void isr_io_bank0() {
	if (enc != NULL && encoder_irq(enc)) return;
	if (front != NULL && ultrasonic_irq(front)) return;
	//if (side != NULL && ultrasonic_irq(side)) return;
	log("Unrecognised IO bank0 interrupt!");
}

void setup() {
	motor_l = create_motor(19, 18);
	motor_r = create_motor(21, 20);
	enc = create_encoder(3);
	front = create_ultrasonic(7, 6);
	//side = create_ultrasonic(9, 8);

	nvic_hw->icpr = BIT(IO_IRQ_BANK0);
	nvic_hw->iser = BIT(IO_IRQ_BANK0);
}

uint32_t loop_len = 10000000;
uint32_t last_time = 0;

void loop() {
	update_encoder(enc);
	update_ultrasonic(front);
	//update_ultrasonic(side);

	uint32_t time = us_count();
	float speed = (float)(loop_len - time % loop_len) / loop_len;
	//set_speed(motor_l, speed);
	//set_speed(motor_r, speed);
	
	if (time - last_time >= 100000) {
		last_time = time;
		log("%f %f %u", speed, get_speed(enc), get_milimeters(front));
	}
}
