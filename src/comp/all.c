#include "all.h"

#include "../my_hardware/utils.h"
#include "../my_hardware/adc.h"
#include "../my_hardware/timer.h"
#include "src/comp/motor.h"

#include <hardware/regs/intctrl.h>
#include <hardware/structs/nvic.h>

motor_t *motor_l;
motor_t *motor_r;
float max_speed_l;
float max_speed_r;

encoder_t *enc;

ultrasonic_t *front;

uint16_t ir_l;
uint16_t ir_r;

#include "../logging.h"

extern volatile void isr_io_bank0() {
	if (enc != NULL && encoder_irq(enc)) return;
	if (front != NULL && ultrasonic_irq(front)) return;
	log("Unrecognised IO bank0 interrupt!");
}

void setup_components() {
	PIN_SET_TYPE(PIN_LED, SIO);
	SIO_SET_OUT(PIN_LED);

	motor_l = create_motor(18, 19, -0.851259f,4.951442,-0.826466f);
	motor_r = create_motor(21, 20, -0.768897f,4.468736f,-0.654518f);
	max_speed_l = get_max_speed(motor_l);
	max_speed_r = get_max_speed(motor_r);

	enc = create_encoder(3);
	front = create_ultrasonic(7, 6);

	adc_out[1] = &ir_l;
	adc_out[0] = &ir_r;
	adc_set_sample_rate(1000);
	adc_start_looping();
	
	nvic_hw->icpr = BIT(IO_IRQ_BANK0);
	nvic_hw->iser = BIT(IO_IRQ_BANK0);
	nvic_hw->icpr = BIT(ADC_IRQ_FIFO);
	nvic_hw->iser = BIT(ADC_IRQ_FIFO);
}

void update_components() {
	update_encoder(enc, get_direction(motor_r));
	update_ultrasonic(front);
	adc_sanity_check();

	if ((us_count() & 0xfffff) < 0x80000) {
		SIO_SET(PIN_LED);
	}
	else {
		SIO_CLEAR(PIN_LED);
	}
}