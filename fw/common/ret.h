#pragma once
#include <stdint.h>

uint8_t ret_reg_read(uint8_t addr);
void ret_reg_write(uint8_t addr, uint8_t data);
void ret_reg_write_mask(uint8_t addr, uint8_t data, uint8_t mask);
