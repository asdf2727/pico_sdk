#pragma once

typedef enum {
	FOLLOW,
	INTER_ROT,
	AVOID_TURN,
	AVOID_MOVE,
	HALT
} state_t;

extern state_t state;