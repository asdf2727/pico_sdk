#include "timer.h"

#include <hardware/structs/timer.h>

inline uint32_t us_count() {
	return timer_hw->timerawl;
}
inline uint64_t us_count_long() {
	return ((uint64_t)timer_hw->timerawh << 32) | timer_hw->timerawl;
}

// These are good enough, but they hog the bus and stop DMA from doing anything for the entire duration of the sleep
// Consider using SysTick to solve this
void my_busy_wait_us(uint32_t delay_us) {
	uint32_t target_us = timer_hw->timerawl + delay_us;
	while (timer_hw->timerawl != target_us) {}
}
void my_busy_wait_us_long(uint64_t delay_us) {
	uint64_t target_us = us_count_long() + delay_us;
	uint32_t target_high = target_us >> 32;
	uint32_t target_low = target_us;
	while (timer_hw->timerawh < target_high) {}
	while (timer_hw->timerawl < target_low) {}
}
void my_busy_wait_ms(uint32_t delay_ms) {
	my_busy_wait_us_long((uint64_t)delay_ms * 1000);
}
void my_busy_wait_s(uint32_t delay_s) {
	my_busy_wait_us_long((uint64_t)delay_s * 1000000);
}