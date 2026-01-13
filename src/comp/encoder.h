
#pragma once

#include <stdint.h>

struct encoder_t;
typedef struct encoder_t encoder_t;

encoder_t *create_encoder(int pin);
void delete_encoder(encoder_t *enc);

int encoder_irq(encoder_t *enc);

void update_encoder(encoder_t *enc);

float get_speed(encoder_t *enc);
int32_t get_spokes(encoder_t *enc);
int32_t get_angle(encoder_t *enc);
int32_t get_turns(encoder_t *enc);