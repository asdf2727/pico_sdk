#pragma once

#include "../my_hardware/sio.h"

#include "motor.h"
#include "encoder.h"
#include "ultrasonic.h"

#define PIN_LED 25

extern motor_t *motor_l;
extern motor_t *motor_r;

extern encoder_t *enc; // On L motor

extern ultrasonic_t *front;

extern uint16_t ir_l;
extern uint16_t ir_r;
