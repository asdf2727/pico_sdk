#pragma once

#include <stdint.h>

extern uint16_t *adc_out[5];

void adc_start_looping();

void adc_stop_looping();

void adc_set_sample_rate(float freq);

void adc_sanity_check();