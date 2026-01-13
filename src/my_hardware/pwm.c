#include "pwm.h"

#include <hardware/structs/pwm.h>
#include <math.h>

#include "pll.h"
#include "utils.h"

void pwm_set_freq(int slice_no, float target_freq) {
	pwm_slice_hw_t *slice = &pwm_hw->slice[slice_no];
	float div = sys_freq() / target_freq;

	if (div > 0x1000000) {
		div *= 0.5f;
		REG_RW_SET1(slice->csr, PWM_CH0_CSR_PH_CORRECT, 1);
	}
	else {
		REG_RW_SET1(slice->csr, PWM_CH0_CSR_PH_CORRECT, 0);
	}

	assert(div <= 0x1000000);
	uint32_t mult = ceilf(div / 0xffff); // assume maximum top for more precision in pulse width
	if (mult < 0x10) mult = 0x10;
	if (mult > 0xfff) mult = 0xfff;
	REG_RW_SET(slice->div, 0xfff, mult); // write to int and frac at the same time

	uint32_t max = nearbyintf(div / ((float)mult / 16));
	if (max < 1) max = 1;
	if (max > 0xffff) max = 0xffff;
	REG_RW_SET1(slice->top, PWM_CH0_TOP, max);

	slice->cc = 0;
}

void pwm_set_enabled(int slice_no, int enabled) {
	pwm_slice_hw_t *slice = &pwm_hw->slice[slice_no];
	REG_RW_SET1(slice->csr, PWM_CH0_CSR_EN, enabled);
}

void pwm_set_duty_cycle(int pin_no, float duty_cycle) {
	//assert(duty_cycle >= 0);
	pwm_slice_hw_t *slice = &pwm_hw->slice[PWM_PIN_TO_SLICE(pin_no)];
	uint32_t threshold = nearbyintf(REG_GET(slice->top, PWM_CH0_TOP) * duty_cycle);
	if (pin_no & 1) {
		REG_RW_SET1(slice->cc, PWM_CH0_CC_B, threshold);
	}
	else {
		REG_RW_SET1(slice->cc, PWM_CH0_CC_A, threshold);
	}
}

float pwm_get_duty_cycle(int pin_no) {
	pwm_slice_hw_t *slice = &pwm_hw->slice[PWM_PIN_TO_SLICE(pin_no)];
	uint32_t cycle = REG_GET(slice->top, PWM_CH0_TOP);
	if (pin_no & 1) {
		return (float)REG_GET(slice->cc, PWM_CH0_CC_B) / cycle;
	}
	else {
		return (float)REG_GET(slice->cc, PWM_CH0_CC_A) / cycle;
	}
}