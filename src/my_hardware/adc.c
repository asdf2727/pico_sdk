#include "adc.h"

#include <math.h>

#include <hardware/regs/adc.h>
#include <hardware/structs/adc.h>
#include <hardware/regs/intctrl.h>

#include "utils.h"

uint16_t *adc_out[5];

volatile extern void isr_adc_fifo() {
	#pragma unroll
	for (int i = 0; i < 5; i++) {
		if (adc_out[i] == NULL) continue;
		*adc_out[i] = adc_hw->fifo;
	}
}

void adc_stop_looping();

void adc_start_looping() {
	int first_bit = -1;
	int bit_count = 0;
	int mask = 0;
	for (int bit = 0; bit < 5; bit++) {
		if (adc_out[bit] == NULL) continue;
		mask |= 1 << bit;
		if (first_bit == -1) first_bit = bit;
		bit_count++;
	}
	adc_stop_looping();
	if (mask == 0) return;
	uint32_t reg = 0;
	reg |= REG_VAL(ADC_FCS_THRESH, 4 + (bit_count >> 1));
	reg |= REG_VAL(ADC_FCS_OVER, 1);
	reg |= REG_VAL(ADC_FCS_UNDER, 1);
	reg |= REG_VAL(ADC_FCS_ERR, 1);
	reg |= REG_VAL(ADC_FCS_EN, 1);
	adc_hw->fcs = reg;
	reg = 0;
	reg |= REG_VAL(ADC_CS_RROBIN, mask);
	reg |= REG_VAL(ADC_CS_AINSEL, first_bit);
	reg |= REG_VAL(ADC_CS_TS_EN, adc_out[4] != NULL);
	reg |= REG_VAL(ADC_CS_START_MANY, 1);
	reg |= REG_VAL(ADC_CS_EN, 1);
	adc_hw->cs = reg;
	adc_hw->inte = 1;
}

#include "../logging.h"

void adc_stop_looping() {
	if (!adc_hw->cs) return;
	adc_hw->cs = 0;
	adc_hw->inte = 0;
	while (REG_GET(adc_hw->fcs, ADC_FCS_LEVEL)) {
		adc_hw->fifo;
	}
}

void adc_set_sample_rate(float freq) {
	uint32_t val = nearbyintf(500000 * 0x100 / freq - 0x100);
	if (val > 0xffffff) val = 0xffffff;
	adc_hw->div = (val);
}

void adc_sanity_check() {
	if (REG_GET(adc_hw->fcs, ADC_FCS_OVER) | REG_GET(adc_hw->fcs, ADC_FCS_UNDER)) {
		adc_start_looping();
		logf("%u %u %u", REG_GET(adc_hw->fcs, ADC_FCS_LEVEL), REG_GET(adc_hw->fcs, ADC_FCS_OVER), REG_GET(adc_hw->fcs, ADC_FCS_UNDER));
	}
}