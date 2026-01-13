#pragma once

struct motor_t;
typedef struct motor_t motor_t;


motor_t *create_motor(int pinA, int pinB);
void delete_motor(motor_t *motor);

void set_speed(motor_t *motor, float speed);
float est_speed(motor_t *motor);
int get_direction(motor_t *motor);