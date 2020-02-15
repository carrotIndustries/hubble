#include "view.h"

static uint8_t view_current = 0;
static const view_t **views;
static uint8_t n_views;

static const uint8_t view_led_index = 4;

void view_init(const view_t **aviews, uint8_t n)
{
    views = aviews;
    n_views = n;
    event_t ev = {.type = EVENT_ENTER};
    views[view_current]->main(&ev);
}

static uint8_t view_is_available(uint8_t view)
{
    return views[view]->is_available == 0 || views[view]->is_available();
}

static void switch_view(uint8_t next_view)
{
    next_view %= n_views;
    if (view_is_available(next_view)) {
        event_t ev = {.type = EVENT_LEAVE};
        views[view_current]->main(&ev);
        view_current = next_view;
        ev.type = EVENT_ENTER;
        views[view_current]->main(&ev);
    }
}

void view_handle_event(const event_t *event)
{
    if (event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_MODE) {
        uint8_t next_view = (view_current + 1) % n_views;
        while (!view_is_available(next_view)) {
            next_view = (next_view + 1) % n_views;
        }
        switch_view(next_view);
        return;
    }
    else if (event->type == EVENT_LED_ON) {
        switch_view(view_led_index);
        views[view_current]->main(event);
    }
    else {
        views[view_current]->main(event);
    }
}
