#pragma once
#include <stdint.h>

void ret_hw_set_power_enable(uint8_t p);
uint8_t ret_hw_get_power_enable(void);

uint8_t ret_hw_reg_read(uint8_t addr);
void ret_hw_reg_write(uint8_t addr, uint8_t data);
