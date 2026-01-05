#pragma once

#include <stdint.h>

uint32_t us_count();
uint64_t us_count_long();

void my_busy_wait_us(uint32_t delay_us);
void my_busy_wait_us_long(uint64_t delay_us);
void my_busy_wait_ms(uint32_t delay_ms);
void my_busy_wait_s(uint32_t delay_s);