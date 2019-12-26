#include "ret.h"
#include "ret_hw.h"

uint8_t ret_reg_read(uint8_t addr)
{
    return ret_hw_reg_read(addr);
}

void ret_reg_write(uint8_t addr, uint8_t data)
{
    ret_hw_reg_write(addr, data);
}

void ret_reg_write_mask(uint8_t addr, uint8_t data, uint8_t mask)
{
    uint8_t t = ret_reg_read(addr);
    t &= ~mask;
    t |= (mask & data);
    ret_reg_write(addr, t);
}
