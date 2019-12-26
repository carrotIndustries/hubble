#include "common/dpy_hw.h"

static uint8_t brightness = 15;

void dpy_hw_set_brightness(uint8_t b) {
	brightness = b;
}

uint8_t dpy_hw_get_brightness(void) {
	return brightness;
}
