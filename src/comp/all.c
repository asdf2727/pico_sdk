#include "all.h"

#include "../my_hardware/utils.h"
#include "../my_hardware/timer.h"

#include <hardware/regs/intctrl.h>
#include <hardware/structs/nvic.h>

motor_t *motor_l;
motor_t *motor_r;

encoder_t *enc;

ultrasonic_t *front;

#include "../logging.h"

extern volatile void isr_io_bank0() {
	if (enc != NULL && encoder_irq(enc)) return;
	if (front != NULL && ultrasonic_irq(front)) return;
	log("Unrecognised IO bank0 interrupt!");
}

void setup_components() {
	PIN_SET_TYPE(PIN_LED, SIO);
	SIO_SET_OUT(PIN_LED);

	motor_l = create_motor(21, 20, 0,12.502281f,-4.298415f);
	motor_r = create_motor(18, 19, 0,10.730250f,-3.923384f);

	enc = create_encoder(3);
	front = create_ultrasonic(7, 6);
	//side = create_ultrasonic(9, 8);
	
	nvic_hw->icpr = BIT(IO_IRQ_BANK0);
	nvic_hw->iser = BIT(IO_IRQ_BANK0);

	setup_infrared();
}

void update_components() {
	update_encoder(enc, get_direction(motor_r));
	update_ultrasonic(front);
	update_infrared();

	if ((us_count() & 0xfffff) < 0x80000) {
		SIO_SET(PIN_LED);
	}
	else {
		SIO_CLEAR(PIN_LED);
	}
}