#include "views.h"
#include "common/dpy.h"
#include "common/sfp.h"
#include "common/view_util.h"
#include "common/view_timer.h"
#include "common/sfp_hw.h"
#include "common/util.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

static sfp_mon_t g_mon;

static sfp_alarm_cal_t sfp_alarm_cal;


static void swap2(void *p)
{
    uint8_t *p8 = p;
    uint8_t t = p8[0];
    p8[0] = p8[1];
    p8[1] = t;
}

static void swap4(void *p)
{
    uint8_t *p8 = p;
    uint8_t t = p8[0];
    p8[0] = p8[3];
    p8[3] = t;
    t = p8[1];
    p8[1] = p8[2];
    p8[2] = t;
}

static void fix_cal(sfp_alarm_cal_t *a)
{
    swap4(&a->rx_pwr4);
    swap4(&a->rx_pwr3);
    swap4(&a->rx_pwr2);
    swap4(&a->rx_pwr1);
    swap4(&a->rx_pwr0);
    swap2(&a->t_offset);
    swap2(&a->tx_pwr_offset);
}


static void fix_mon(sfp_mon_t *m)
{
    swap2(&m->temp);
    swap2(&m->vcc);
    swap2(&m->rx_power);
    swap2(&m->tx_power);
    swap2(&m->tx_bias);
}

static uint16_t get_rx_power(const sfp_mon_t *mon)
{
    if (sfp_sid_get()->dm_type & (1 << 4)) { // externally calibrated
        uint16_t r = mon->rx_power;
        return roundf(sfp_alarm_cal.rx_pwr4 * r * r * r * r + sfp_alarm_cal.rx_pwr3 * r * r * r
                      + sfp_alarm_cal.rx_pwr2 * r * r + sfp_alarm_cal.rx_pwr1 * r + sfp_alarm_cal.rx_pwr0);
    }
    else { // must be interally calibrated
        return mon->rx_power;
    }
}

float sfp_fix_to_float(sfp_fix_t fix)
{
    return fix.integ + fix.frac / 256.f;
}

static uint16_t get_tx_power(const sfp_mon_t *mon)
{
    if (sfp_sid_get()->dm_type & (1 << 4)) { // externally calibrated
        uint16_t r = mon->tx_power;
        return sfp_alarm_cal.tx_pwr_offset + sfp_fix_to_float(sfp_alarm_cal.tx_pwr_slope) * r;
    }
    else { // must be interally calibrated
        return mon->tx_power;
    }
}

static int16_t get_temp(const sfp_mon_t *mon)
{
    if (sfp_sid_get()->dm_type & (1 << 4)) { // externally calibrated
        uint16_t r = mon->temp;
        return ((sfp_alarm_cal.t_offset + sfp_fix_to_float(sfp_alarm_cal.t_slope) * r) * 10) / 256;
    }
    else { // must be interally calibrated
        return (mon->temp * 10) / 256;
    }
}

static uint8_t item_sel = 0;
static uint8_t show_dBm = 1;

static void view_pwr_main(const event_t *event)
{
    dpy_clear();
    view_draw_header("MON");
    if (event->type == EVENT_ENTER) {
        view_timer_start(500);
    }
    else if (event->type == EVENT_LEAVE) {
        view_timer_stop();
    }
    else if (event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_DOWN) {
        INC_MOD(item_sel, 2);
    }
    else if (event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_UP) {
        DEC_MOD(item_sel, 2);
    }
    else if (event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_SELECT) {
        if (item_sel == 0) {
            show_dBm = !show_dBm;
        }
        else if (item_sel == 1) {
            sfp_hw_set_tx_dis(!sfp_hw_get_tx_dis());
        }
    }
    if (sfp_state_get() == SFP_STATE_READY) {
        uint8_t r_alarm = sfp_hw_read(0xa2, 0, sizeof(sfp_alarm_cal_t), &sfp_alarm_cal);
        if (r_alarm == SFP_READ_OK) {
            fix_cal(&sfp_alarm_cal);
        }

        memset(&g_mon, 0, sizeof(sfp_mon_t));
        if (sfp_hw_read(0xa2, 96, sizeof(sfp_mon_t), &g_mon) != SFP_READ_OK) {
            memset(&g_mon, 0, sizeof(sfp_mon_t));
        }
        fix_mon(&g_mon);

        uint16_t rxp = get_rx_power(&g_mon);
        dpy_set_font(DPY_FONT_24_BOLD);
        if (show_dBm) {
            int16_t dBm = sfp_uw_to_dBm(rxp);
            dpy_puti_dp_signed(12, 8, 3, 1, dBm);
            dpy_puts(6 * 12, 8, "dBm");
            if (item_sel == 0)
                dpy_invert_rect(6 * 12, 8, 3 * 12, 24);
        }
        else {
            dpy_puti_dp(0, 8, 5, 1, rxp);
            dpy_puts(6 * 12, 8, "µW");
            if (item_sel == 0)
                dpy_invert_rect(6 * 12, 8, 2 * 12, 24);
        }
        dpy_set_font(DPY_FONT_8);
        dpy_puts(0, 8 + 24, "TX is ");
        if (!sfp_hw_get_tx_dis()) {
            dpy_puts(36, 8 + 24, "ON");
            if (item_sel == 1)
                dpy_invert_rect(36, 8 + 24, 6 * 2, 8);
        }
        else {
            dpy_puts(36, 8 + 24, "OFF");
            if (item_sel == 1)
                dpy_invert_rect(36, 8 + 24, 6 * 3, 8);
        }

        {
            uint16_t txp = get_tx_power(&g_mon);
            if (show_dBm) {
                int16_t dBm = sfp_uw_to_dBm(txp);
                dpy_puti_dp_signed(64 + 6, 36, 3, 1, dBm);
                dpy_puts(64 + 6 * 6, 36, "dBm");
            }
            else {
                dpy_puti_dp(64, 36, 5, 1, txp);
                dpy_puts(64 + 6 * 6, 36, "µW");
            }
        }
        {
            int16_t temperature = get_temp(&g_mon);
            dpy_puti_dp(0, 44, 3, 1, temperature);
            dpy_puts(4 * 6, 44, "°C");
        }
        {
            uint8_t st = sfp_hw_read_status();
            if (st & SFP_STATUS_TX_FAULT)
                dpy_puts(8 * 6, 44, "TX FAULT");

            if (st & SFP_STATUS_LOS)
                dpy_puts(18 * 6, 44, "LOS");
        }

        // dpy_puti(0, 8+24, 5, g_mon.vcc);
    }
    else {
        dpy_set_font(DPY_FONT_16_BOLD);
        dpy_puts(8, 24, "no transceiver");
    }
}

static uint8_t view_pwr_is_available()
{
    return sfp_state_get() == SFP_STATE_READY && sfp_hw_has_mon();
}


const view_t view_pwr = {
        .main = view_pwr_main,
        .is_available = view_pwr_is_available,
};
