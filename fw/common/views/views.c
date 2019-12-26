#include "views.h"
#include "../util.h"
void view_pwr_main(const event_t *event);
void view_id_main(const event_t *event);
void view_led_main(const event_t *event);
void view_set_main(const event_t *event);
void view_i2c_main(const event_t *event);
void view_phy_main(const event_t *event);
void view_eye_main(const event_t *event);


static const view_t view_id = {
    .main = view_id_main
};

static const view_t view_led = {
    .main = view_led_main
};

static const view_t view_set = {
    .main = view_set_main
};

static const view_t view_i2c = {
    .main = view_i2c_main
};

static const view_t view_phy = {
    .main = view_phy_main
};

static const view_t view_pwr = {
    .main = view_pwr_main
};

static const view_t view_eye = {
    .main = view_eye_main
};

const view_t *the_views[] = {
    &view_id,
	&view_pwr,
	&view_eye,
    &view_phy,
	&view_led,
	&view_set,
	&view_i2c,
};

const uint8_t the_n_views = ARRAY_SIZE(the_views);
