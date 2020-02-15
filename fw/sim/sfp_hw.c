#include "common/sfp_hw.h"

uint8_t sfp_hw_try_lock(void)
{
    return 1;
}
void sfp_hw_unlock(void)
{
}
void sfp_hw_req(uint8_t op)
{
}

uint8_t sfp_hw_read(uint8_t i2c_addr, uint8_t mem_addr, uint8_t len, void *dest)
{
    return SFP_READ_OK;
}
static uint8_t tx_dis = 0;
void sfp_hw_set_tx_dis(uint8_t dis)
{
    tx_dis = dis;
}
uint8_t sfp_hw_get_tx_dis(void)
{
    return tx_dis;
}

uint8_t sfp_hw_read_status(void)
{
    return 0;
}

static uint8_t sfp_auto_on = 0;

void sfp_hw_set_auto_on(uint8_t v)
{
    sfp_auto_on = v;
}
uint8_t sfp_hw_get_auto_on()
{
    return sfp_auto_on;
}
