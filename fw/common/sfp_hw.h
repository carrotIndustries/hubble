#pragma once
#include <stdint.h>

typedef enum { SFP_REQ_SCAN = 1 } sfp_req_t;

uint8_t sfp_hw_try_lock(void);
void sfp_hw_unlock(void);
void sfp_hw_req(uint8_t op);

typedef enum {
    SFP_READ_OK,
    SFP_READ_ERR,
    SFP_READ_TIMEOUT
} sfp_read_result_t;

uint8_t sfp_hw_read(uint8_t i2c_addr, uint8_t mem_addr, uint8_t len, void *dest);
void sfp_hw_set_tx_dis(uint8_t dis);
uint8_t sfp_hw_get_tx_dis(void);
void sfp_hw_set_auto_on(uint8_t v);
uint8_t sfp_hw_get_auto_on();

typedef enum {
	SFP_STATUS_LOS = (1<<0),
	SFP_STATUS_TX_FAULT = (1<<1)
} sfp_status_t;

uint8_t sfp_hw_read_status(void);
