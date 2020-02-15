#pragma once
#include <stdint.h>
#include "event.h"

typedef struct {
	void (*main)(const event_t *event);
	uint8_t (*is_available)();
} view_t;

void view_init(const view_t **aviews, uint8_t n);
void view_handle_event(const event_t *event);
