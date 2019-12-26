#include "auto_off.h"

static uint8_t auto_off_timeout = 2;

uint8_t auto_off_get_timeout_min(void) {
	return auto_off_timeout;
}

void auto_off_set_timeout_min(uint8_t t) {
	auto_off_timeout = t;
}

static uint8_t auto_off_timer = 0;
static uint8_t auto_off_charge_timer = 0;

void auto_off_set_charge_mode(void) {
	auto_off_charge_timer = 1;
}

uint8_t auto_off_get_charge_mode(void) {
	return !!auto_off_charge_timer;
}

void auto_off_handle_event(const event_t *event){
	if(event->type == EVENT_SLEEP_TIMER) { //every 5 sec
		if(auto_off_timeout) {
			auto_off_timer++;
			if(auto_off_timer > auto_off_timeout*12) {
				auto_off_timer = 0;
				send_event(EVENT_POWER_OFF, 0);
			}
		}
		if(auto_off_charge_timer) {
			auto_off_charge_timer--;
			if(auto_off_charge_timer == 0) {
				send_event(EVENT_POWER_OFF, 0);
			}
		}
	}
	else if(event->type == EVENT_BUTTON) {
		auto_off_timer = 0;
		auto_off_charge_timer = 0;
	}
}


