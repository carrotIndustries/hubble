#include "views.h"
#include "common/dpy.h"
#include "common/led.h"
#include "common/view_util.h"
#include <stdlib.h>
#include <string.h>

void view_led_main(const event_t *event)
{
    dpy_clear();
    view_draw_header("LED");
    if (event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_SELECT) {
        led_set_state(!led_get_state());
    }
    else if (event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_UP) {
        led_set_level(led_get_level()+1);
    }
    else if (event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_DOWN) {
        led_set_level(led_get_level()-1);
    }
    dpy_set_font(DPY_FONT_32_BOLD);
    if (led_get_state()) {
        dpy_puts(0, 16, "ON");
    }
    else {
        dpy_puts(0, 16, "OFF");
    }
    dpy_puti(104, 16, 1, led_get_level());
    dpy_set_font(DPY_FONT_8);
    dpy_puts(68, 24, "POWER");
    dpy_puts(68, 32, "LEVEL");
}
