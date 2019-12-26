#include "font.h"

uint8_t font_get_bytes_per_char(const font_t *font) {
	return font->height/8*font->width;
}
