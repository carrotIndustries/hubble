#include "led.h"
#include "led_hw.h"
#include "util.h"

static uint8_t state = 0;
static uint8_t level = LED_LEVEL_MIN;

static const uint16_t led_levels[LED_LEVEL_MAX - LED_LEVEL_MIN + 1] = {512, 1024, 4096, 8192, 32768, 65535};

static void update()
{
    led_hw_set(state * led_levels[level - LED_LEVEL_MIN]);
}

void led_set_state(uint8_t on)
{
    state = !!on;
    update();
}
uint8_t led_get_state(void)
{
    return state;
}
void led_set_level(uint8_t lev)
{
    level = CLAMP(lev, LED_LEVEL_MIN, LED_LEVEL_MAX);
    update();
}
uint8_t led_get_level(void)
{
    return level;
}
