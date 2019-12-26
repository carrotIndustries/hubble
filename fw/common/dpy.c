#include <stdint.h>
#include "dpy.h"
#include "util.h"
#include <string.h>
#include "font.h"
#include "trg_conf.h"

#ifdef TRG_BOOTLOADER
#include "font_u8_small.h"
#else
#include "font_u24b.h"
#include "font_u32b.h"
#include "font_u16b.h"
#include "font_u16n.h"
#include "font_u8.h"
#endif

static uint8_t framebuffer[DPY_HEIGHT * DPY_WIDTH / 8];

typedef struct {
    px_t offset_x;
    px_t offset_y;
    dpy_mode_t mode;
    dpy_text_mode_t text_mode;
} state_t;

static state_t state;

void dpy_clear(void)
{
    memset(framebuffer, 0, sizeof(framebuffer));
    dpy_set_font(DPY_FONT_8);
}

uint8_t *dpy_get_fb(void)
{
    return framebuffer;
}

size_t dpy_get_fb_size(void)
{
    return sizeof(framebuffer);
}

static uint8_t shiftone(uint8_t x)
{
    return 1 << x;
}

static inline void set_px(px_t x, px_t y)
{
    uint8_t page = y / 8;
    uint8_t bit = y % 8;
    framebuffer[page * DPY_WIDTH + x] |= shiftone(bit);
}

static inline void clear_px(px_t x, px_t y)
{
    uint8_t page = y / 8;
    uint8_t bit = y % 8;
    framebuffer[page * DPY_WIDTH + x] &= ~shiftone(bit);
}

static inline void draw_px(px_t x, px_t y)
{
    uint8_t page = y / 8;
    uint8_t bit = y % 8;
    switch (state.mode) {
    case DPY_MODE_CLR:
        framebuffer[page * DPY_WIDTH + x] &= ~shiftone(bit);
        break;
    case DPY_MODE_SET:
        framebuffer[page * DPY_WIDTH + x] |= shiftone(bit);
        break;
    case DPY_MODE_XOR:
        framebuffer[page * DPY_WIDTH + x] ^= shiftone(bit);
        break;
    }
}

static inline void xl_x(px_t *x)
{
    *x = MIN(*x + state.offset_x, DPY_WIDTH - 1);
}

static inline void xl_y(px_t *y)
{
    *y = MIN(*y + state.offset_y, DPY_HEIGHT - 1);
}

void dpy_hline(px_t x1, px_t y1, px_t x2)
{
    xl_x(&x1);
    xl_x(&x2);
    xl_y(&y1);
    px_t start = MIN(x1, x2);
    px_t end = MAX(x1, x2);
    for (px_t i = start; i < end; i++) {
        set_px(i, y1);
    }
}

void dpy_translate(pxs_t x, pxs_t y)
{
    state.offset_x += x;
    state.offset_y += y;
}

void dpy_set_text_inverse(uint8_t inverse)
{
}

const font_t *font_current = &font_u8;

void dpy_set_font(uint8_t font)
{
#ifndef TRG_BOOTLOADER
    switch (font) {
    case DPY_FONT_32_BOLD:
        font_current = &font_u32b;
        break;

    case DPY_FONT_24_BOLD:
        font_current = &font_u24b;
        break;

    case DPY_FONT_16_BOLD:
        font_current = &font_u16b;
        break;

    case DPY_FONT_16:
        font_current = &font_u16n;
        break;

    default:
        font_current = &font_u8;
    }
#endif
}


void dpy_putc(uint8_t x, uint8_t y, char c)
{
    uint8_t page = y / 8;
    if (page > 7)
        return;
    const uint8_t *font_data = font_current->data + font_get_bytes_per_char(font_current) * c;

    for (uint8_t iy = 0; iy < font_current->height / 8; iy++) {
        memcpy(framebuffer + (128 * page) + (128 * iy) + x, font_data + (iy * font_current->width),
               font_current->width);
    }
}

void dpy_puts(uint8_t x, uint8_t y, const char *s)
{
    while (*s) {
        dpy_putc(x, y, *s);
        x += font_current->width;
        s++;
    }
}

void dpy_putsn(uint8_t x, uint8_t y, const char *s, uint8_t len)
{
    while (*s && len) {
        dpy_putc(x, y, *s);
        x += font_current->width;
        s++;
        len--;
    }
}

static char hex2ascii(uint8_t v)
{
    if (v < 10) {
        return '0' + v;
    }
    else {
        return 'a' + v - 10;
    }
}

void dpy_putix(uint8_t x, uint8_t y, uint8_t len, uint32_t value)
{
    x += (len - 1) * font_current->width;
    while (len--) {
        dpy_putc(x, y, hex2ascii(value & 0xf));
        value >>= 4;
        x -= font_current->width;
    }
}

void dpy_puti(uint8_t x, uint8_t y, uint8_t len, uint32_t value)
{
    x += (len - 1) * font_current->width;
    while (len--) {
        dpy_putc(x, y, '0' + (value % 10));
        value /= 10;
        x -= font_current->width;
    }
}

void dpy_puti_dp(uint8_t x, uint8_t y, uint8_t len, uint8_t fracs, uint32_t value)
{
    len++;
    x += (len - 1) * font_current->width;
    fracs = len - fracs-1;
    while (len--) {
        if (len == fracs) {
            dpy_putc(x, y, '.');
        }
        else {
            dpy_putc(x, y, '0' + (value % 10));
            value /= 10;
        }
        x -= font_current->width;
    }
}

void dpy_puti_dp_signed(uint8_t x, uint8_t y, uint8_t len, uint8_t fracs, int32_t value) {
	dpy_puti_dp(x+font_current->width, y, len, fracs, ABS(value));
	dpy_putc(x, y, value<0?'-':'+');
}

void dpy_set_text_mode(dpy_text_mode_t mode)
{
    state.text_mode = mode;
}

void dpy_invert_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h)
{
    for (uint8_t iy = y / 8; iy < (h + y) / 8; iy++) {
        for (uint8_t ix = x; ix < x + w; ix++) {
            framebuffer[iy * 128 + ix] ^= 0xff;
        }
    }
}
