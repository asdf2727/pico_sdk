#pragma once

#include "../my_hardware/sio.h"

#include "motor.h"
#include "encoder.h"
#include "ultrasonic.h"
#include "infrared.h"

#define PIN_LED 25

extern motor_t *motor_l;
extern motor_t *motor_r;

extern encoder_t *enc;

extern ultrasonic_t *front;

extern float ir_offset;
extern int ir_status;
