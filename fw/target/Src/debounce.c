#include "debounce.h"
#include "main.h"
#include "os.h"
#include "common/event.h"

#define REPEAT_MASK (0xf) // repeat: key1, key2
#define REPEAT_START 50   // after 500ms
#define REPEAT_NEXT 10    // every 200ms

static uint8_t key_state; // debounced and inverted key state:
                          // bit = 1: key pressed
static uint8_t key_press; // key press detect

static uint8_t key_rpt;

static uint8_t get_key_press(uint8_t key_mask)
{

    key_mask &= key_press; // read key(s)
    key_press ^= key_mask; // clear key(s)
    return key_mask;
}

static uint8_t get_key_short(uint8_t key_mask)
{
    return get_key_press(~key_state & key_mask);
}

static uint8_t get_key_rpt(uint8_t key_mask)
{
    key_mask &= key_rpt; // read key(s)
    key_rpt ^= key_mask; // clear key(s)
    return key_mask;
}

uint8_t get_key_long(uint8_t key_mask)
{
    return get_key_press(get_key_rpt(key_mask));
}

static uint8_t wakeup = 0;

void trg_debounce(void)
{
    uint8_t key_pin = 0xff;
    if (!HAL_GPIO_ReadPin(BTN_DOWN_GPIO_Port, BTN_DOWN_Pin)) {
        key_pin &= ~(1 << 0);
    }
    if (!HAL_GPIO_ReadPin(BTN_UP_GPIO_Port, BTN_UP_Pin)) {
        key_pin &= ~(1 << 1);
    }
    if (!HAL_GPIO_ReadPin(BTN_MODE_GPIO_Port, BTN_MODE_Pin)) {
        key_pin &= ~(1 << 2);
    }
    if (!HAL_GPIO_ReadPin(BTN_SEL_GPIO_Port, BTN_SEL_Pin)) {
        key_pin &= ~(1 << 3);
    }
    if (wakeup) {
        if (key_pin == 0xff)
            wakeup = 0;
        else
            return;
    }
    static uint8_t ct0 = 0xFF, ct1 = 0xFF, rpt;
    uint8_t i;

    i = key_state ^ ~key_pin;   // key changed ?
    ct0 = ~(ct0 & i);           // reset or count ct0
    ct1 = ct0 ^ (ct1 & i);      // reset or count ct1
    i &= ct0 & ct1;             // count until roll over ?
    key_state ^= i;             // then toggle debounced state
    key_press |= key_state & i; // 0->1: key press detect

    if ((key_state & REPEAT_MASK) == 0) // check repeat function
        rpt = REPEAT_START;             // start delay
    if (--rpt == 0) {
        rpt = REPEAT_NEXT; // repeat delay
        key_rpt |= key_state & REPEAT_MASK;
    }

    event_t ev = {.type = EVENT_BUTTON};
    ev.param = EVENT_BUTTON_NONE;
    if (get_key_short(1 << 0) || get_key_rpt(1 << 0)) {
        ev.param = EVENT_BUTTON_DOWN;
    }
    else if (get_key_long(1 << 0)) {
        ev.param = EVENT_BUTTON_DOWN | EVENT_BUTTON_LONG;
    }
    else if (get_key_short(1 << 1) || get_key_rpt(1 << 1)) {
        ev.param = EVENT_BUTTON_UP;
    }
    else if (get_key_long(1 << 1)) {
        ev.param = EVENT_BUTTON_UP | EVENT_BUTTON_LONG;
    }
    else if (get_key_short(1 << 2)) {
        ev.param = EVENT_BUTTON_MODE;
    }
    else if (get_key_long(1 << 2)) {
        ev.param = EVENT_BUTTON_MODE | EVENT_BUTTON_LONG;
    }
    else if (get_key_short(1 << 3)) {
        ev.param = EVENT_BUTTON_SELECT;
    }
    else if (get_key_long(1 << 3)) {
        ev.param = EVENT_BUTTON_SELECT | EVENT_BUTTON_LONG;
    }
    if (ev.param != EVENT_BUTTON_NONE) {
        xQueueSend(event_queue, &ev, 0);
    }
}

void debounce_enter_sleep(void)
{
}

void debounce_exit_sleep(void)
{
    wakeup = 1;
}
