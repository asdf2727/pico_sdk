#pragma once

//#define DISABLE_LOGGING

#ifdef DISABLE_LOGGING

#define log(format)
#define logf(format, ...)

#else

#include <stdio.h>
#include "my_hardware/timer.h"

#define log(format) { \
	printf("%llu: " format "\n", us_count_long()); \
}
#define logf(format, ...) { \
	printf("%llu: " format "\n", us_count_long(), __VA_ARGS__); \
}
#define logcsv(format, ...) { \
	printf("%llu," format "\n", us_count_long(), __VA_ARGS__); \
}

#endif