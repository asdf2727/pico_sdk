#include "ultrasonic.h"

#include <stdlib.h>

#include "../my_hardware/irq.h"
#include "src/my_hardware/sio.h"
#include "src/my_hardware/timer.h"

struct ultrasonic_t {
	int trig_pin;
	int echo_pin;
	uint32_t last_send;
	uint32_t last_receive;
	uint32_t prev_receive;
	uint32_t next_set;
	uint32_t next_reset;
	uint32_t mms;
};

ultrasonic_t *create_ultrasonic(int trig_pin, int echo_pin) {
	IRQ_ENABLE(echo_pin, 4 | 8);
	PIN_SET_TYPE(trig_pin, SIO);
	SIO_SET_OUT(trig_pin);

	ultrasonic_t *sonic = malloc(sizeof(ultrasonic_t));
	sonic->trig_pin = trig_pin;
	sonic->echo_pin = echo_pin;
	sonic->last_send = 0;
	sonic->last_receive = 0;
	sonic->prev_receive = 0;
	sonic->next_set = 0;
	sonic->next_reset = 0;
	sonic->mms = 0;
	return sonic;
}

void delete_ultrasonic(ultrasonic_t *sonic) {
	IRQ_DISABLE(sonic->echo_pin);
	free(sonic);
}

int ultrasonic_irq(ultrasonic_t *sonic) {
	if (IRQ_STATUS(sonic->echo_pin, 8)) {
		IRQ_CLEAR(sonic->echo_pin, 8);
		sonic->last_send = us_count();
		return true;
	}
	if (IRQ_STATUS(sonic->echo_pin, 4)) {
		IRQ_CLEAR(sonic->echo_pin, 4);
		sonic->last_receive = us_count();
		return true;
	}
	return false;
}

void update_ultrasonic(ultrasonic_t *sonic) {
	uint32_t time = us_count();
	if (sonic->last_receive != sonic->prev_receive) {
		sonic->prev_receive = sonic->last_receive;
		uint32_t delta = sonic->last_receive - sonic->last_send;
		sonic->mms = delta * 343 / 2000;
		//if (sonic->next_set > time + 3 * delta) {
		//	sonic->next_set = time + 3 * delta;
		//	sonic->next_reset = sonic->next_set + 11;
		//}
		return;
	}
	if (sonic->next_set < time) {
		sonic->next_set = time + 60000;
		SIO_SET(sonic->trig_pin);
	}
	if (sonic->next_reset < time) {
		sonic->next_reset = sonic->next_set + 11;
		SIO_CLEAR(sonic->trig_pin);
	}
}

inline uint32_t get_milimeters(ultrasonic_t *sonic) {
	return sonic->mms;
}