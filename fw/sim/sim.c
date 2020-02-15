#include <stdio.h>
#include <zmq.h>
#include <stdint.h>
#include <string.h>
#include "common/dpy.h"
#include "common/util.h"
#include "common/view_timer.h"
#include "common/views/views.h"
#include "common/event.h"
// gcc -std=c11  sim.c Graphics.c menu2/menu.c menu2/img.c  menu2/menu_defs.c -lzmq -o sim -g3 -Wall

void view_timer_start(uint16_t interval)
{
}

void view_timer_stop()
{
}

void send_event(uint8_t type, uint8_t param) {

}

void enter_bootloader(void) {}

int main(void)
{
    void *ctx = zmq_ctx_new();
    void *sock = zmq_socket(ctx, ZMQ_REP);
    zmq_bind(sock, "ipc://displaysim");
    char buf;
    view_init(the_views, the_n_views);
    while (1) {
        zmq_recv(sock, &buf, 1, 0);
        event_t ev;
        ev.type = EVENT_NONE;
        switch (buf) {
        case 'm':
            ev.type = EVENT_BUTTON;
            ev.param = EVENT_BUTTON_MODE;
            break;

        case 's':
            ev.type = EVENT_BUTTON;
            ev.param = EVENT_BUTTON_SELECT;
            break;

        case 'u':
            ev.type = EVENT_BUTTON;
            ev.param = EVENT_BUTTON_UP;
            break;

        case 'd':
            ev.type = EVENT_BUTTON;
            ev.param = EVENT_BUTTON_DOWN;
            break;
        }
        view_handle_event(&ev);

        printf("rx %c\n", buf);
        zmq_send(sock, dpy_get_fb(), dpy_get_fb_size(), 0);
    }
}
