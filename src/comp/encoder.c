#include "encoder.h"

#include <math.h>
#include <stdlib.h>

#include "../my_hardware/irq.h"
#include "../my_hardware/timer.h"

struct encoder_t {
	uint32_t prev2_update;
	uint32_t prev_update;
	uint32_t last_update;
	uint32_t last_delta;
	float speed;
	int32_t angle;
	int pin;
};

encoder_t *create_encoder(int pin) {
	io_bank0_hw->proc0_irq_ctrl.inte[pin >> 3] |= IRQ_PIN_BITS(4 | 8, pin);

	encoder_t *enc = malloc(sizeof(encoder_t));
	enc->prev2_update = 0;
	enc->prev_update = 0;
	enc->last_update = 0;
	enc->last_delta = 0;
	enc->speed = 0;
	enc->angle = 0;
	enc->pin = pin;
	return enc;
}

void delete_encoder(encoder_t *enc) {
	io_bank0_hw->proc0_irq_ctrl.inte[enc->pin >> 3] &= ~IRQ_PIN_BITS(4 | 8, enc->pin);
	free(enc);
}

inline int encoder_irq(encoder_t *enc) {
	if (!(io_bank0_hw->proc0_irq_ctrl.ints[enc->pin >> 3] & IRQ_PIN_BITS(4 | 8, enc->pin))) return false;
	io_bank0_hw->intr[enc->pin >> 3] = IRQ_PIN_BITS(4 | 8, enc->pin);

	enc->last_update = us_count();
	return true;
}

//#include "../logging.h"

#define SPOKES 20
#define SMOOTH 0

void update_encoder(encoder_t *enc) {
	if (enc->prev_update == enc->last_update) return;
	if (enc->last_update - enc->prev_update < 1000) {
		//log("Encoder glitch %u %u", enc->last_update - enc->prev_update, enc->last_delta);
		enc->last_update = enc->prev_update;
		return;
	}
	enc->last_delta = enc->last_update - enc->prev2_update;
	enc->prev2_update = enc->prev_update;
	enc->prev_update = enc->last_update;
	enc->speed = SMOOTH * enc->speed + (float)((1 - SMOOTH) * M_PI * 500000) / (SPOKES * enc->last_delta);
	enc->angle += 1;
}

#undef SMOOTH

float get_speed(encoder_t *enc) {
	float speed = enc->speed;
	uint32_t time = us_count();
	if (time - enc->prev_update > 2 * enc->last_delta) {
		return 0; //speed * (float)enc->last_delta / (time - enc->prev_update);
	}
	return speed;
}
inline int32_t get_spokes(encoder_t *enc) {
	return enc->angle;
}
// Consider adding speed term to make it continuous
inline int32_t get_angle(encoder_t *enc) {
	return enc->angle * (float)M_PI / SPOKES;
}
inline int32_t get_turns(encoder_t *enc) {
	return enc->angle / SPOKES;
}

// TODO compute regression of speed given duty cycle