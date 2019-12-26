#include "common/ret_hw.h"

static uint8_t power_on = 0;

void ret_hw_set_power_enable(uint8_t p)
{
	power_on = p;
}
uint8_t ret_hw_get_power_enable(void)
{
    return power_on;
}

uint8_t ret_hw_reg_read(uint8_t addr)
{
    return 0;
}

void ret_hw_reg_write(uint8_t addr, uint8_t data)
{

}
