#include "common/m88e111.h"
#include "common/sfp.h"
#include "common/m88e111_hw.h"
#include "m88e111_trg.h"
#include <string.h>

static I2C_HandleTypeDef *hi2c = 0;

void m88e111_trg_init(I2C_HandleTypeDef *h)
{
    hi2c = h;
}

static uint8_t last_error;

uint16_t m88e111_hw_reg_read(uint8_t addr)
{
    uint8_t r[2];
    last_error = HAL_I2C_Mem_Read(hi2c, SFP_I2C_ADDR_88E111, addr, 1, (void *)&r, 2, 1000);
    return r[0] << 8 | r[1];
}

void m88e111_hw_reg_write(uint8_t addr, uint16_t data)
{
    uint8_t d[2];
    d[0] = (data >> 8);
    d[1] = data & 0xff;
    last_error = HAL_I2C_Mem_Write(hi2c, SFP_I2C_ADDR_88E111, addr, 1, (void *)&d, 2, 1000);
}

uint8_t m88e111_hw_get_last_error(void)
{
    return last_error;
}
