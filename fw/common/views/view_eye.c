#include "views.h"
#include "common/dpy.h"
#include "common/view_util.h"
#include "common/ret_hw.h"
#include "common/ret.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

static uint8_t r = 0;

static uint32_t freq = 10312500;

static uint8_t r63, r62, r64;

static void update(void) {
        ret_reg_write(0xff, 0x05);
        const float f0 = 25e6;
        const float f1 = freq*1e3;
        const float N  = 1024;
        const float f2 = f1/32.;
        const float f3 = f2 * N / f0;
        uint16_t f4 = roundf(f3);
        r63 = (1<<7)|((f4>>8)&0x7f);
        ret_reg_write(0x63, r63);
        r62 = f4&0xff;
        ret_reg_write(0x62, r64);
        uint8_t f5 = f4/1000;
        r64 = f5;
        ret_reg_write_mask(0x64, r64, 0x0f);
        
        ret_reg_write(0x0a, 0x1c);
        ret_reg_write(0x0a, 0x10);
}

void view_eye_main(const event_t *event)
{
    dpy_clear();
    view_draw_header("EYE");

    if (event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_SELECT) {
        ret_hw_set_power_enable(1);
        ret_reg_write(0xff, 0x00);
        r = ret_hw_reg_read(1);
        ret_reg_write(0xff, 0x04);
        ret_reg_write_mask(0x00, 0x04, 0x04);
        ret_reg_write_mask(0x14, 0x80, 0x80);
        ret_reg_write_mask(0x09, 0x04, 0x04);
        ret_reg_write_mask(0x09, 0x80, 0x80);
        ret_reg_write_mask(0x08, 0x12, 0x1f);
        ret_reg_write_mask(0x18, 0x00, 0x70); // divider 1
        ret_reg_write_mask(0x09, 0x08, 0x08);
        ret_reg_write_mask(0x1b, 0x00, 0x03);
        ret_reg_write_mask(0x1f, 0x12, 0x1f);
        ret_reg_write_mask(0x1e, 0x10, 0x10);
        ret_reg_write_mask(0x30, 0x08, 0x0f);
        ret_reg_write_mask(0x09, 0x20, 0x20);
        ret_reg_write_mask(0x1e, 0x80, 0xe0);
        ret_reg_write_mask(0x0d, 0x20, 0x20);
        
        
        
        ret_reg_write(0xff, 0x05); //channel b
        ret_reg_write_mask(0x00, 0x04, 0x04); //reset
        ret_reg_write(0x2f, 0xf6);
        ret_reg_write(0x60, 0x00);
        ret_reg_write(0x61, 0xb2);
        ret_reg_write(0x62, 0x90);
        ret_reg_write(0x63, 0xb3);
        ret_reg_write(0x64, 0xcd);
        
        ret_reg_write(0x0a, 0x1c);
        ret_reg_write(0x0a, 0x10);
       
    }
    else if (event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_DOWN) {
        if (ret_hw_get_power_enable()) {
                freq -= 10000;
                update();
                //ret_reg_write(0xff, 0x04);
                //ret_reg_write(0x08, ret_reg_read(0x08)-1);
        }
    }
    else if (event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_UP) {
        if (ret_hw_get_power_enable()) {
                freq += 10000;
                update();
                //ret_reg_write(0xff, 0x04);
                //ret_reg_write(0x08, ret_reg_read(0x08)+1);
        }
    }
    if (ret_hw_get_power_enable()) {
        dpy_puts(0, 8, "ON");
        dpy_putix(0, 16, 2, r);

        ret_reg_write(0xff, 0x05);
        dpy_putix(32, 16, 2, ret_reg_read(0x02));
        dpy_putix(64, 16, 2, ret_reg_read(0x54));
        
        
        //dpy_putix(0, 24, 2, ret_reg_read(0x08));
        dpy_puti_dp(0, 24, 9, 6, freq);
        dpy_putix(64, 24, 2, r62);
        dpy_putix(64+18, 24, 2, r63);
        dpy_putix(64+18+18, 24, 2, r64);
    }
}
