#pragma once
#include <stdint.h>

typedef enum {
    EVENT_NONE,
    EVENT_ENTER,
    EVENT_LEAVE,
    EVENT_BUTTON,
    EVENT_SFP,
    EVENT_BAT,
	EVENT_TIMER,
	EVENT_POWER_OFF,
	EVENT_SLEEP_TIMER,
	EVENT_LED_ON,
} event_type_t;

typedef enum {
    EVENT_BUTTON_NONE,
    EVENT_BUTTON_MODE,
    EVENT_BUTTON_SELECT,
    EVENT_BUTTON_UP,
    EVENT_BUTTON_DOWN,
    EVENT_BUTTON_LONG = (1 << 7)
} event_param_button_t;

typedef enum {
    EVENT_SFP_INSERTED,
    EVENT_SFP_READY,
    EVENT_SFP_ERROR,
    EVENT_SFP_DATA,
    EVENT_SFP_REMOVED,
    EVENT_SFP_SCAN_DONE,
} event_param_sfp_t;

typedef struct {
    uint8_t type;
    uint8_t param;
} event_t;

 void send_event(uint8_t type, uint8_t param);
