#include "views.h"
#include "common/dpy.h"
#include "common/sfp.h"
#include "common/sfp_hw.h"
#include "common/view_util.h"
#include "common/util.h"
#include <stdlib.h>
#include <string.h>

void view_i2c_main(const event_t *event)
{
    dpy_clear();
    view_draw_header("I2C");
    if (event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_SELECT) {
        if (sfp_state_get() != SFP_STATE_NOT_PRESENT) {
            sfp_hw_req(SFP_REQ_SCAN);
        }
    }
    /*else if (event->type == EVENT_SFP && event->param == EVENT_SFP_INSERTED) {
        sfp_i2c_scan_reset();
        sfp_hw_req(SFP_REQ_SCAN);
    }
    else if (event->type == EVENT_SFP && event->param == EVENT_SFP_REMOVED) {
        sfp_i2c_scan_reset();
    }
    else if (event->type == EVENT_SFP && event->param == EVENT_SFP_SCAN_DONE) {
    }*/
    if (sfp_state_get() != SFP_STATE_NOT_PRESENT) {
        const sfp_i2c_scan_t *res = sfp_i2c_scan_get();
        dpy_puts(0, 8, "7  8   6543210");
        for (uint8_t i = 0; i < ARRAY_SIZE(res->addrs); i++) {
            if (res->addrs[i] != 0xff) {
                uint8_t a = res->addrs[i];
                uint8_t y = 8 * (i + 2);
                dpy_putix(0, y, 2, a);
                dpy_putix(18, y, 2, a << 1);
                for (uint8_t bit = 0; bit < 7; bit++) {
                    char c = '0' + !!(a & (1 << bit));
                    dpy_putc(13*6-bit*6, y, c);
                }
            }
        }
    }
    else {
        dpy_set_font(DPY_FONT_16_BOLD);
        dpy_puts(8, 24, "no transceiver");
    }
}
