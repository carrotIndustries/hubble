#include "views.h"
#include "common/dpy.h"
#include "common/sfp.h"
#include "common/sfp_hw.h"
#include "common/m88e111.h"
#include "common/m88e111_hw.h"
#include "common/view_util.h"
#include "common/view_timer.h"
#include "common/util.h"
#include <stdlib.h>
#include <string.h>

static uint8_t frame_count;
static uint16_t phy_status;

static uint8_t item_sel = 1;
static uint8_t item_sel_tdr = 0;

static uint8_t subview = 0;

static uint16_t vct_status[4];
static uint16_t last_vct_status;

void view_phy_main(const event_t *event)
{
    dpy_clear();
    view_draw_header("PHY");
    if (event->type == EVENT_ENTER) {
        view_timer_start(500);
        if (sfp_model_get() == SFP_MODEL_88E111) {
            m88e111_reg_sbi(M88E111_REG_TEST_ENABLE_CTRL, M88E111_REG_TEST_ENABLE_CTRL_CRC);
        }
    }
    else if (event->type == EVENT_LEAVE) {
        view_timer_stop();
    }
    else if (event->type == EVENT_SFP && event->param == EVENT_SFP_READY) {
        if (sfp_model_get() == SFP_MODEL_88E111) {
            m88e111_reg_sbi(M88E111_REG_TEST_ENABLE_CTRL, M88E111_REG_TEST_ENABLE_CTRL_CRC);
        }
    }
    if (subview == 0) {
        if (event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_DOWN) {
            INC_MOD(item_sel, 2);
        }
        else if (event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_UP) {
            DEC_MOD(item_sel, 2);
        }
        else if (event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_SELECT) {
            if (item_sel == 0) {
                m88e111_reg_cbi(M88E111_REG_TEST_ENABLE_CTRL, M88E111_REG_TEST_ENABLE_CTRL_CRC);
                m88e111_reg_sbi(M88E111_REG_TEST_ENABLE_CTRL, M88E111_REG_TEST_ENABLE_CTRL_CRC);
            }
            if (item_sel == 1) {
                subview = 1;
                item_sel_tdr = 0;
            }
        }
    }
    else if (subview == 1) {
        if (event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_DOWN) {
            INC_MOD(item_sel_tdr, 2);
        }
        else if (event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_UP) {
            DEC_MOD(item_sel_tdr, 2);
        }
        else if (event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_SELECT) {
            if (item_sel_tdr == 0) {
            	if (sfp_model_get() == SFP_MODEL_88E111) {
                if (!(m88e111_reg_read(M88E111_REG_VCT0) & M88E111_REG_VCT0_RUN)) {
                    m88e111_reg_sbi(M88E111_REG_VCT0, M88E111_REG_VCT0_RUN);
                }
            	}
            }
            if (item_sel_tdr == 1) {
                subview = 0;
            }
        }
    }

    if (sfp_state_get() == SFP_STATE_READY) {
        if (sfp_model_get() == SFP_MODEL_88E111) {
            if (subview == 0) {
                if (event->type == EVENT_TIMER) {
                    phy_status = m88e111_reg_read(M88E111_REG_PHY_STATUS);
                    frame_count = m88e111_reg_read(M88E111_REG_CRC_RESULT) >> 8;
                }
                if (phy_status & M88E111_REG_PHY_STATUS_LINK) {
                    dpy_set_font(DPY_FONT_16_BOLD);
                    dpy_puts(0, 8, "Link");
                    dpy_set_font(DPY_FONT_8);
                    if (phy_status & M88E111_REG_PHY_STATUS_AUTONEG_RESOLVED) {
                        uint8_t speed = (phy_status >> 14) & ONES(2);
                        const char *sspeed = "   ?";
                        const char *speeds[] = {"  10", " 100", "1000"};
                        if (speed < ARRAY_SIZE(speeds)) {
                            sspeed = speeds[speed];
                        }

                        dpy_set_font(DPY_FONT_16_BOLD);
                        dpy_puts(36, 8, sspeed);

                        dpy_puts(36 + 4 * 8 + 6, 8, "Mb/s");
                        dpy_set_font(DPY_FONT_8);
                        const char *dpx = "HDX";
                        if (phy_status & M88E111_REG_PHY_STATUS_FDX) {
                            dpx = "FDX";
                        }
                        dpy_puts((5 + 5 + 5) * 6, 24, dpx);
                        if (phy_status & M88E111_REG_PHY_STATUS_MDIX) {
                            dpy_puts((5 + 5 + 5 + 4) * 6, 24, "X");
                        }


                        uint8_t length = (phy_status >> 7) & ONES(3);
                        const char *lengths[] = {"< 50m", "50 - 80m", "80 - 110m", "110 - 140m", "> 140m"};
                        const char *slength = "?";
                        if (length < ARRAY_SIZE(lengths)) {
                            slength = lengths[length];
                        }
                        dpy_puts(0, 56, "Length:");
                        dpy_puts((6 * 7), 56, slength);
                    }
                    else {
                        dpy_puts(5 * 6, 8, "Autoneg...");
                    }
                }
                else {
                    dpy_set_font(DPY_FONT_16_BOLD);
                    dpy_puts(0, 8, "No Link");
                    dpy_set_font(DPY_FONT_8);
                }
                dpy_puts(0, 24, "Frames RX:");
                dpy_puti(10 * 6, 24, 3, frame_count);
                if (item_sel == 0) {
                    dpy_invert_rect(10 * 6, 24, 3 * 6, 8);
                }
                dpy_puts(0, 40, ">TDR");
                if (item_sel == 1) {
                    dpy_invert_rect(0, 40, 4 * 6, 8);
                }
            }
            else {

                dpy_puts(0, 8, "Start");
                if (item_sel_tdr == 0) {
                    dpy_invert_rect(0, 8, 5 * 6, 8);
                }
                dpy_puts(6 * 6, 8, "Exit");
                if (item_sel_tdr == 1) {
                    dpy_invert_rect(6 * 6, 8, 4 * 6, 8);
                }
                if (event->type == EVENT_TIMER) {
                    uint16_t status = m88e111_reg_read(M88E111_REG_VCT0);
                    if ((!(status & M88E111_REG_VCT0_RUN))
                        && (last_vct_status & M88E111_REG_VCT0_RUN)) { // has finished
                        for (uint8_t i = 0; i < 4; i++) {
                            uint8_t max_repeat = 0xff;
                            do {
                                vct_status[i] = m88e111_reg_read(M88E111_REG_VCT(i));
                                max_repeat--;
                            } while ((vct_status[i] & 0xff) == 0xff && max_repeat);
                        }
                    }
                    last_vct_status = status;
                }
                if (last_vct_status & M88E111_REG_VCT0_RUN) {
                    dpy_set_font(DPY_FONT_16_BOLD);
                    dpy_puts(8, 32, "TDR running...");
                    dpy_set_font(DPY_FONT_8);
                }
                else {
                    uint8_t y = 16 + 8;
                    for (uint8_t i = 0; i < 4; i++) {
                        dpy_puts(0, y, "MDI");
                        dpy_puti(3 * 6, y, 1, i);
                        uint16_t status = vct_status[i];
                        uint8_t s = (status >> 13) & ONES(2);
                        if (s == 3) {
                            dpy_puts(5 * 6, y, "Fail");
                        }
                        else {
                            if (s == 0) {
                                dpy_puts(5 * 6, y, "norm");
                            }
                            else if (s == 1) {
                                dpy_puts(5 * 6, y, "short");
                            }
                            else if (s == 2) {
                                dpy_puts(5 * 6, y, "open");
                            }

                            // dpy_puti((5+6) * 6, y, 3, status&0xff);
                            dpy_putix((5 + 6) * 6, y, 4, status);

                            // dpy_puts((5+6+3) * 6, y, "m");
                        }

                        y += 8;
                    }
                }
                // dpy_putix(0, 56, 2, m88e111_hw_get_last_error());
            }
        }
        else {
            dpy_set_font(DPY_FONT_16_BOLD);
            dpy_puts(8, 24, "incompatible");
        }
    }
    else {
        dpy_set_font(DPY_FONT_16_BOLD);
        dpy_puts(8, 24, "no transceiver");
    }
}
