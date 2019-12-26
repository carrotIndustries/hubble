#pragma once
#include <stdint.h>

uint16_t m88e111_hw_reg_read(uint8_t addr);
void m88e111_hw_reg_write(uint8_t addr, uint16_t data);
uint8_t m88e111_hw_get_last_error(void);
