#pragma once
#include <stdint.h>
#include <stddef.h>

#define DPY_WIDTH 128
#define DPY_HEIGHT 64

typedef uint8_t px_t;
typedef int8_t pxs_t;
#define PX_MAX 255

typedef enum {
	DPY_MODE_SET,
	DPY_MODE_CLR,
	DPY_MODE_XOR
} dpy_mode_t;

typedef enum {
	DPY_TEXT_MODE_BG,
	DPY_TEXT_MODE_BG_INV,
} dpy_text_mode_t;

typedef enum {
	DPY_FONT_8,
	DPY_FONT_16,
	DPY_FONT_16_BOLD,
	DPY_FONT_24_BOLD,
	DPY_FONT_32_BOLD,
} dpy_font_t;

void dpy_clear();

uint8_t *dpy_get_fb(void);
size_t dpy_get_fb_size(void);

void dpy_set_font(uint8_t font);

void dpy_puts(uint8_t x, uint8_t y, const char *s);
void dpy_putsn(uint8_t x, uint8_t y, const char *s, uint8_t len);
void dpy_putc(uint8_t x, uint8_t y, char c);
void dpy_putix(uint8_t x, uint8_t y, uint8_t len, uint32_t value);
void dpy_puti(uint8_t x, uint8_t y, uint8_t len, uint32_t value);
void dpy_puti_dp(uint8_t x, uint8_t y, uint8_t len, uint8_t fracs, uint32_t value);
void dpy_puti_dp_signed(uint8_t x, uint8_t y, uint8_t len, uint8_t fracs, int32_t value);
void dpy_hline(px_t x1, px_t y1, px_t x2);
void dpy_set_text_mode(dpy_text_mode_t mode);
void dpy_invert_rect(uint8_t x, uint8_t y, uint8_t w, uint8_t h);
