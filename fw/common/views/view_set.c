#include "views.h"
#include "common/view_util.h"
#include "common/dpy.h"
#include "common/dpy_hw.h"
#include "common/dfu_hw.h"
#include "common/sfp_hw.h"
#include "common/util.h"
#include "common/auto_off.h"
#include <stdlib.h>
#include <string.h>

typedef enum { SETTING_TYPE_INT, SETTING_TYPE_ACTION, SETTING_TYPE_BOOL } setting_type_t;

#define SETTING_LINE_HEAD                                                                                              \
    const char *label;                                                                                                 \
    uint8_t type;

typedef struct {
    SETTING_LINE_HEAD
} setting_line_unknown_t;

typedef struct {
    SETTING_LINE_HEAD
    int16_t (*get)(void);
    void (*set)(int16_t v);
    int16_t min;
    int16_t max;
    uint8_t digits;
    uint8_t flags;
} setting_line_int_t;

typedef enum {
    SETTING_LINE_FLAG_0_OFF = (1 << 0),
} setting_line_flags_t;

typedef struct {
    SETTING_LINE_HEAD
    void (*cb)(void);
} setting_line_action_t;

typedef struct {
    SETTING_LINE_HEAD
    uint8_t (*get)(void);
    void (*set)(uint8_t v);
} setting_line_bool_t;

static int16_t bright_get(void)
{
    return dpy_hw_get_brightness();
}

static void bright_set(int16_t v)
{
    dpy_hw_set_brightness(v);
}

static int16_t aoff_get(void)
{
    return auto_off_get_timeout_min();
}

static void aoff_set(int16_t v)
{
    auto_off_set_timeout_min(v);
}


static const setting_line_int_t li_brightness = {.label = "Disp. brightness",
                                                 .type = SETTING_TYPE_INT,
                                                 .get = bright_get,
                                                 .set = bright_set,
                                                 .min = 0,
                                                 .max = 15,
                                                 .digits = 2};

static const setting_line_int_t li_auto_off = {.label = "Auto off (min)",
                                               .type = SETTING_TYPE_INT,
                                               .get = aoff_get,
                                               .set = aoff_set,
                                               .min = 0,
                                               .max = 10,
                                               .digits = 2,
                                               .flags = SETTING_LINE_FLAG_0_OFF};

static const setting_line_bool_t li_auto_on_sfp = {
        .label = "Power up on SFP",
        .type = SETTING_TYPE_BOOL,
        .get = sfp_hw_get_auto_on,
        .set = sfp_hw_set_auto_on,
};


static const setting_line_action_t li_dfu = {.label = "Enter Bootloader",
                                             .type = SETTING_TYPE_ACTION,
                                             .cb = enter_bootloader};
static const setting_line_unknown_t *lines[] = {
        (void *)(&li_brightness),
        (void *)(&li_auto_off),
        (void *)(&li_auto_on_sfp),
        (void *)(&li_dfu),
};

const setting_line_unknown_t *get_line(uint8_t index)
{
    if (index < ARRAY_SIZE(lines)) {
        return lines[index];
    }
    else {
        return 0;
    }
}

static uint8_t item_current = 0;
static uint8_t item_offset = 0;
uint8_t edit_mode = 0;

void view_set_main(const event_t *event)
{
    dpy_clear();
    view_draw_header("SET");
    if (edit_mode == 0) {
        if (event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_UP) {
            if (item_current > 0)
                item_current--;
        }
        else if (event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_DOWN) {
            if (item_current < ARRAY_SIZE(lines) - 1)
                item_current++;
        }
        else if (event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_SELECT) {
            if (get_line(item_current)->type == SETTING_TYPE_INT)
                edit_mode = 1;
            else if (get_line(item_current)->type == SETTING_TYPE_ACTION) {
                setting_line_action_t *li = (void *)get_line(item_current);
                li->cb();
            }
            else if (get_line(item_current)->type == SETTING_TYPE_BOOL) {
                setting_line_bool_t *li = (void *)get_line(item_current);
                li->set(!li->get());
            }
        }
    }
    else {
        const setting_line_unknown_t *li = get_line(item_current);
        if (event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_SELECT) {
            edit_mode = 0;
        }
        uint8_t up = event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_UP;
        uint8_t down = event->type == EVENT_BUTTON && event->param == EVENT_BUTTON_DOWN;
        if (li->type == SETTING_TYPE_INT) {
            const setting_line_int_t *li2 = (void *)li;
            if (up) {
                li2->set(MIN(li2->get() + 1, li2->max));
            }
            else if (down) {
                li2->set(MAX(li2->get() - 1, li2->min));
            }
        }
    }
    const uint8_t page_size = 7;
    int8_t item_current_rel = item_current - item_offset;
    if (item_current_rel > page_size - 1) {
        item_offset = item_current;
    }
    if (item_current_rel < 0) {
        item_offset = MAX(item_current - page_size + 1, 0);
    }

    uint8_t y = 8;
    for (uint8_t i = 0; i < page_size; i++) {
        const setting_line_unknown_t *li = get_line(i + item_offset);
        if (li) {
            dpy_puts(0, y, li->label);
            if (li->type == SETTING_TYPE_INT) {
                const setting_line_int_t *li2 = (void *)li;
                int16_t value = li2->get();
                if ((li2->flags & SETTING_LINE_FLAG_0_OFF) && value == 0) {
                    dpy_puts(DPY_WIDTH - 6 * li2->digits, y, "--");
                }
                else {
                    dpy_puti(DPY_WIDTH - 6 * li2->digits, y, li2->digits, li2->get());
                }
            }
            else if (li->type == SETTING_TYPE_BOOL) {
                const setting_line_bool_t *li2 = (void *)li;
                uint8_t value = li2->get();
                if (value) {
                    dpy_puts(DPY_WIDTH - 6 * 3, y, "yes");
                }
                else {
                    dpy_puts(DPY_WIDTH - 6 * 3, y, " no");
                }
            }
        }
        else {
            break;
        }
        if (item_current == i + item_offset) {
            if (edit_mode == 0) {
                dpy_invert_rect(0, y, DPY_WIDTH, 8);
            }
            else {
                uint8_t w = 0;
                if (li->type == SETTING_TYPE_INT) {
                    const setting_line_int_t *li2 = (void *)li;
                    w = li2->digits;
                }
                w *= 6;
                dpy_invert_rect(DPY_WIDTH - w, y, w, 8);
            }
        }
        y += 8;
    }
}
