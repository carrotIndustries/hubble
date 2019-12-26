#pragma once
#include <stdint.h>

typedef struct {
    uint8_t width;
    uint8_t height;
    const uint8_t *data;
} font_t;

uint8_t font_get_bytes_per_char(const font_t *font);
