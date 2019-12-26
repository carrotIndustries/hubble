#include "view_util.h"
#include "sfp.h"
#include "bat_hw.h"
#include "dpy.h"

void view_draw_header(const char *view_name) {
    dpy_puts(0,0, view_name);
    sfp_state_t state = sfp_state_get();
    if(state == SFP_STATE_NOT_PRESENT) {
        dpy_puts(24,0, "NO SFP");
    }
    else if(state == SFP_STATE_PRESENT) {
        dpy_puts(24,0, "SFP?");
    }
    else if(state == SFP_STATE_READY) {
        dpy_puts(24,0, "SFP");
    }
    else if(state == SFP_STATE_ERROR) {
        dpy_puts(24,0, "SFP ERR");
    }

    dpy_puti_dp(86, 0, 3, 2, bat_hw_read()/10);
    dpy_putc(86+4*6, 0, 'V');
    uint8_t bat_state = bat_hw_get_state();
    if(bat_state & BAT_STATE_CHG) {
    	dpy_putc(86+5*6, 0, 'c');
    }
    if(bat_state & BAT_STATE_PG) {
    	dpy_putc(86+6*6, 0, 'p');
    }

}
