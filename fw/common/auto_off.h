#pragma once
#include <stdint.h>
#include "event.h"

void auto_off_handle_event(const event_t *event);
uint8_t auto_off_get_timeout_min(void);
void auto_off_set_timeout_min(uint8_t t);
void auto_off_set_charge_mode(void);
uint8_t auto_off_get_charge_mode(void);
