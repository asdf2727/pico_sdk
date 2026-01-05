#include <hardware/structs/sio.h>
#include <hardware/structs/io_bank0.h>

#include "my_hardware/utils.h"
#include "my_hardware/pwm.h"
#include "my_hardware/timer.h"

#define LED 25
#define M1A 19
#define M1B 18
#define M2A 21
#define M2B 20

#define BIT(bit) (1ul << (bit))

#define PIN_SET_TYPE(pin, TYPE) io_bank0_hw->io[pin].ctrl = REG_VAL(IO_BANK0_GPIO0_CTRL_FUNCSEL, GPIO_FUNC_##TYPE)

void setup() {
	PIN_SET_TYPE(LED, SIO);
	PIN_SET_TYPE(M1B, SIO);
	PIN_SET_TYPE(M2B, SIO);
	sio_hw->gpio_oe_set = BIT(LED) | BIT(M1B) | BIT(M2B);
	sio_hw->gpio_clr = BIT(LED) | BIT(M1B) | BIT(M2B);
	
	PIN_SET_TYPE(M1A, PWM);
	pwm_set_freq(PWM_PIN_TO_SLICE(M1A), 1000);
	pwm_set_enabled(PWM_PIN_TO_SLICE(M1A), 1);
	PIN_SET_TYPE(M2A, PWM);
	pwm_set_freq(PWM_PIN_TO_SLICE(M2A), 1000);
	pwm_set_enabled(PWM_PIN_TO_SLICE(M2A), 1);
}

#include <stdio.h>

uint32_t loop_len = 5000000;

void loop() {
	uint32_t time = us_count();
	printf("%u\n", time);
	pwm_set_duty_cycle(M1A, (float)(loop_len - time % loop_len) / loop_len);
	pwm_set_duty_cycle(M2A, (float)(loop_len - time % loop_len) / loop_len);
	my_busy_wait_ms(100);
}
