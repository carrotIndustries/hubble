#pragma once
#include <stdint.h>

typedef enum {
	BAT_STATE_PG = 1,
	BAT_STATE_CHG = 2
} bat_state_t;

uint16_t  bat_hw_read(void);
uint8_t bat_hw_get_state(void);
