#pragma once

#include <stdint.h>

struct motor_t;
typedef struct motor_t motor_t;

#define WHEEL_RADIUS 3.25f // cm
#define MIN_DUTY 0.3f

motor_t *create_motor(int pinA, int pinB, float a, float b, float c);
void delete_motor(motor_t *motor);

void set_duty(motor_t *motor, float duty);
void set_speed(motor_t *motor, float speed);

float est_speed(motor_t *motor);
int get_direction(motor_t *motor);
float get_max_speed(motor_t *motor);