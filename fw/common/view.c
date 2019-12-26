#include "view.h"

static uint8_t view_current = 0;
static const view_t **views;
static uint8_t n_views;

void view_init(const view_t **aviews, uint8_t n) {
    views = aviews;
    n_views = n;
    event_t ev = {.type = EVENT_ENTER};
    views[view_current]->main(&ev);
}

void view_handle_event(const event_t *event) {
    if(event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_MODE) {
        event_t ev = {.type = EVENT_LEAVE};
        views[view_current]->main(&ev);
        view_current = (view_current+1)%n_views;
        ev.type = EVENT_ENTER;
        views[view_current]->main(&ev);
        return;
    }
    else {
        views[view_current]->main(event);
    }
}
