#include "views.h"
#include "common/util.h"
#include "common/dpy.h"
#include "common/view_util.h"
#include "common/sfp.h"
#include "common/sfp_hw.h"
#include <string.h>
//#include "usbd_cdc_if.h"


static char hex2ascii(uint8_t v)
{
    if (v < 10) {
        return '0' + v;
    }
    else {
        return 'a' + v - 10;
    }
}

static void hexs2ascii(char *dst, uint32_t v, uint8_t digits)
{
    while (digits) {
        digits--;
        dst[digits] = hex2ascii(v & 0xf);
        v >>= 4;
    }
}

static void decs2ascii(char *dst, uint32_t v, uint8_t digits)
{
    while (digits) {
        digits--;
        dst[digits] = (v % 10) + '0';
        v /= 10;
    }
    while (*dst && *dst == '0') {
        *dst = ' ';
        dst++;
    }
}

typedef struct {
    const char name_short[4];
    const char name_long[16];
    const char *(*fn)(const sfp_sid_t *sid, const void *user_data);
    const void *user_data;
} line_t;

struct _nv {
    int v;
    const char *n;
};

/* SFF-8472 Rev. 11.4 table 3.4: Connector values */
static struct _nv conn[] = {{0x00, "Unknown"},
                            {0x01, "SC"},
                            {0x02, "Fibre Channel Style 1 copper"},
                            {0x03, "Fibre Channel Style 2 copper"},
                            {0x04, "BNC/TNC"},
                            {0x05, "Fibre Channel coaxial"},
                            {0x06, "FiberJack"},
                            {0x07, "LC"},
                            {0x08, "MT-RJ"},
                            {0x09, "MU"},
                            {0x0A, "SG"},
                            {0x0B, "Optical pigtail"},
                            {0x0C, "MPO Parallel Optic"},
                            {0x20, "HSSDC II"},
                            {0x21, "Copper pigtail"},
                            {0x22, "RJ45"},
                            {0x23, "No separate connector"}, /* SFF-8436 */
                            {0, NULL}};

static struct _nv encoding[] = {{0x00, "Unspecified"}, {0x01, "8B/10B"},          {0x02, "4B/5B"},   {0x03, "NRZ"},
                                {0x04, "Manchester"},  {0x05, "SONET Scrambled"}, {0x06, "64B/66B"}, {0, NULL}};

typedef struct {
    uint8_t offset;
    const struct _nv *lut;
} lookup_info_t;

const char *do_lut(const struct _nv *lut, int index)
{
    while (lut->n) {
        if (lut->v == index) {
            return lut->n;
        }
        lut++;
    }
    return 0;
}

static const char *fn_lookup_byte(const sfp_sid_t *sid, const void *user_data)
{
    const lookup_info_t *info = user_data;
    uint8_t byte = ((const uint8_t *)sid)[info->offset];
    return do_lut(info->lut, byte);
}

static const char *fn_vendor(const sfp_sid_t *sid, const void *user_data)
{
    return sid->vendor_name;
}

static const char *fn_vendor_pn(const sfp_sid_t *sid, const void *user_data)
{
    return sid->vendor_pn;
}

static const char *fn_vendor_serial(const sfp_sid_t *sid, const void *user_data)
{
    return sid->vendor_sn;
}
static const lookup_info_t lut_inf_connector = {2, conn};

static const lookup_info_t lut_inf_encoding = {11, encoding};

static char line_buf[16];

static const char *fn_vendor_oui(const sfp_sid_t *sid, const void *user_data)
{
    uint32_t oui = (sid->vendor_oui[2]) | (sid->vendor_oui[1] << 8) | (sid->vendor_oui[0] << 16);
    memset(line_buf, 0, sizeof(line_buf));
    hexs2ascii(line_buf, oui, 6);
    return line_buf;
}

static const char *fn_vendor_rev(const sfp_sid_t *sid, const void *user_data)
{
    memset(line_buf, 0, sizeof(line_buf));
    memcpy(line_buf, sid->vendor_rev, 4);
    return line_buf;
}


static const char *fn_date_code(const sfp_sid_t *sid, const void *user_data)
{
    memset(line_buf, 0, sizeof(line_buf));
    strcpy(line_buf, "20xx-xx-xx   ");
    memcpy(line_buf + 2, sid->date_code, 2);
    memcpy(line_buf + 5, sid->date_code + 2, 2);
    memcpy(line_buf + 8, sid->date_code + 4, 2);
    memcpy(line_buf + 11, sid->date_code + 6, 2);
    return line_buf;
}

static const char *fn_bit_rate(const sfp_sid_t *sid, const void *user_data)
{
    uint8_t rate = sid->rate;
    memset(line_buf, 0, sizeof(line_buf));
    hexs2ascii(line_buf, rate, 2);
    return line_buf;
}

static const char *fn_lambda(const sfp_sid_t *sid, const void *user_data)
{
    memset(line_buf, 0, sizeof(line_buf));
    decs2ascii(line_buf, sid->wavelength, 5);
    strcpy(line_buf + 5, " nm");
    return line_buf;
}

static const char *fn_dmt(const sfp_sid_t *sid, const void *user_data)
{
    memset(line_buf, ' ', sizeof(line_buf));
    uint8_t d = sid->dm_type;
    if (d & (1 << 6)) {
        line_buf[0] = 'M';
    }
    if (d & (1 << 4)) {
        line_buf[1] = 'E';
    }
    if (d & (1 << 5)) {
        line_buf[2] = 'I';
    }
    if (d & (1 << 2)) {
        line_buf[3] = 'A';
    }

    return line_buf;
}

static const line_t lines[] = {
        {"VEN", "Vendor", fn_vendor, 0},
        {"PN", "Product Number", fn_vendor_pn, 0},
        {"REV", "Revision", fn_vendor_rev, 0},
        {"SN", "Serial", fn_vendor_serial, 0},
        {"Date", "Datecode", fn_date_code, 0},
        {"Rate", "Bit rate", fn_bit_rate, 0},
        {"Wave", "Wavelength", fn_lambda, 0},
        {"Conn", "Connector", fn_lookup_byte, &lut_inf_connector},
        {"Encd", "Encoding", fn_lookup_byte, &lut_inf_encoding},
        {"DMT", "Diag Mon Type", fn_dmt, 0},
        {"OUI", "OUI", fn_vendor_oui, 0},
};

#define N_LINES ARRAY_SIZE(lines)

static char buf[3];
static uint8_t line_offset = 0;
static const int8_t lineskip = 7;

void view_id_main(const event_t *event)
{
    dpy_clear();
    view_draw_header("ID");
    if (event->type == EVENT_BUTTON && (event->param == EVENT_BUTTON_DOWN || event->param == EVENT_BUTTON_UP)) {
        int8_t inc = lineskip * (event->param == EVENT_BUTTON_DOWN ? 1 : -1);
        if (line_offset + inc >= 0 && line_offset + inc <= (N_LINES - 1)) {
            line_offset += inc;
        }
    }
    if (sfp_state_get() == SFP_STATE_READY) {
        if (sfp_hw_try_lock()) {
            for (uint8_t i = 0; i < 7; i++) {
                uint8_t j = i + line_offset;
                if (j < N_LINES) {
                    const line_t *li = &lines[j];
                    const char *r = li->fn(sfp_sid_get(), li->user_data);
                    dpy_putsn(0, (i + 1) * 8, li->name_short, 4);
                    if (r) {
                        dpy_putsn(30, (i + 1) * 8, r, 16);
                    }
                }
            }
            sfp_hw_unlock();
        }
        // dpy_putsn(0, 8, sfp_sid_get()->vendor_name, 16);
        // dpy_putsn(0, 16, sfp_sid_get()->vendor_pn, 16);
    }
    else if (sfp_state_get() == SFP_STATE_PRESENT) {
    	dpy_set_font(DPY_FONT_16_BOLD);
        dpy_puts(8, 24, "please wait...");
    }
    else {
    	dpy_set_font(DPY_FONT_16_BOLD);
        dpy_puts(8, 24, "no transceiver");
    }
    /*if(event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_SELECT) {
        const uint8_t *p = (void*)sfp_sid_get();
        for(uint8_t i = 0; i<sizeof(sfp_sid_t); i++) {
            uint8_t d = p[i];
            buf[0] = hex2ascii(d>>4);
            buf[1] = hex2ascii(d&0xf);
            buf[2] = ' ';
            while(CDC_Transmit_FS(buf, 3) != USBD_OK) {
                ;
            }
        }
        buf[0] = '\n';
        buf[1] = '\n';
        buf[2] = '\n';
        while(CDC_Transmit_FS(buf, 3) != USBD_OK) {
            ;
        }
    }*/
}
