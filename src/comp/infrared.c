#include "infrared.h"

#include <hardware/regs/intctrl.h>
#include <hardware/structs/nvic.h>

#include "../my_hardware/adc.h"
#include "../my_hardware/utils.h"

uint16_t ir_l;
uint16_t ir_r;

void setup_infrared() {
	adc_out[1] = &ir_l;
	adc_out[0] = &ir_r;
	adc_set_sample_rate(10000);
	adc_start_looping();

	nvic_hw->icpr = BIT(ADC_IRQ_FIFO);
	nvic_hw->iser = BIT(ADC_IRQ_FIFO);
}

#define IR_MIN_L 300
#define IR_MAX_L 5000
#define IR_MIN_R 270
#define IR_MAX_R 5000

#define IR_MULT 0.35f

float ir_offset;
int ir_status = 0;
float norm_l, norm_r;

#include "../logging.h"

void update_infrared() {
	adc_sanity_check();
	norm_l = (float)(ir_l - IR_MIN_L) / (IR_MAX_L - IR_MIN_L);
	norm_r = (float)(ir_r - IR_MIN_R) / (IR_MAX_R - IR_MIN_R);
	if (norm_l > 1 || norm_r > 1
		|| 0 > norm_l || 0 > norm_r ||
		norm_l + norm_r < 0.1f) {
		ir_status &= ~1;
		return;
	}

	ir_status |= 1;
	if (norm_r > norm_l && norm_r > 0.4f) ir_status |= 2;
	if (norm_l > norm_r && norm_l > 0.4f) ir_status &= ~2;

	ir_offset = (norm_r - norm_l) * IR_MULT;
}
