#pragma once
#include <stdint.h>

#define LED_LEVEL_MIN 1
#define LED_LEVEL_MAX 6

void led_set_state(uint8_t on);
uint8_t led_get_state(void);
void led_set_level(uint8_t level);
uint8_t led_get_level(void);

