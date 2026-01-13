#pragma once

#include <stdio.h>

#include "my_hardware/timer.h"

#define log(...) printf("%llu: ", us_count_long()); printf(__VA_ARGS__); printf("\n")