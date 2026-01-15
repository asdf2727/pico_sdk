#include <pico/stdio.h>

#include "comp/all.h"

void setup();
void loop();

int main() {
	stdio_init_all();
	setup_components();
	setup();
	while (1) {
		update_components();
		loop();
	}
}
