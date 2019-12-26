#pragma once
#include "main.h"
#include "common/sfp_hw.h"

void sfp_trg_init(I2C_HandleTypeDef *h);
void sfp_trg_set_power(uint8_t on);
uint8_t sfp_trg_is_present(void);
void sfp_trg_i2c_disable(void);
void sfp_trg_i2c_enable(void);
uint8_t sfp_hw_try_lock(void);
void sfp_hw_unlock(void);

uint8_t sfp_trg_read(uint8_t i2c_addr, uint8_t mem_addr, uint8_t len, void *dest);
void sfp_task_code(void *param);

void sfp_trg_enter_sleep(void);
void sfp_trg_exit_sleep(void);
