#include "ret_trg.h"

static I2C_HandleTypeDef *hi2c = 0;
static uint8_t power_on = 0;

void ret_trg_init(I2C_HandleTypeDef *h)
{
    hi2c = h;
}

void ret_hw_set_power_enable(uint8_t p)
{
	__HAL_I2C_DISABLE(hi2c);
    HAL_GPIO_WritePin(RET_PWR_HEN_GPIO_Port, RET_PWR_HEN_Pin, p);
    HAL_Delay(100);
    __HAL_I2C_ENABLE(hi2c);
}
uint8_t ret_hw_get_power_enable(void)
{
    return !!(RET_PWR_HEN_GPIO_Port->ODR & RET_PWR_HEN_Pin);
}

uint8_t ret_hw_reg_read(uint8_t addr)
{
    uint8_t r;
    HAL_I2C_Mem_Read(hi2c, 0x30, addr, 1, &r, 1, 1000);
    return r;
}

void ret_hw_reg_write(uint8_t addr, uint8_t data)
{
	HAL_I2C_Mem_Write(hi2c, 0x30, addr, 1, &data, 1, 1000);
}
