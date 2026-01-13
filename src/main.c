
#include <pico/stdio.h>

void setup();
void loop();

int main() {
	stdio_init_all();
	setup();
	while (1) { loop(); }
}
