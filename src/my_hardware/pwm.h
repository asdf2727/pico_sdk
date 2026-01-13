#pragma once

#define PWM_PIN_TO_SLICE(pin) ((pin & 15) >> 1)

void pwm_set_freq(int slice_no, float target_freq);

void pwm_set_enabled(int slice_no, int enabled);

void pwm_set_duty_cycle(int pin_no, float duty_cycle);
float pwm_get_duty_cycle(int pin_no);