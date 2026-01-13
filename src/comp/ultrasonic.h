#pragma once

#include <stdint.h>

struct ultrasonic_t;
typedef struct ultrasonic_t ultrasonic_t;

ultrasonic_t *create_ultrasonic(int trig_pin, int echo_pin);

void delete_ultrasonic(ultrasonic_t *sonic);

int ultrasonic_irq(ultrasonic_t *sonic);

void update_ultrasonic(ultrasonic_t *sonic);

uint32_t get_milimeters(ultrasonic_t *sonic);