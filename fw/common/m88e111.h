#pragma once
#include <stdint.h>


#define M88E111_REG(reg, page) ((reg&0xff) | ((page&0xff)<<8))

#define M88E111_REG_PHHYID0 0x02
#define M88E111_REG_PHHYID1 0x03

#define M88E111_REG_PHY_STATUS 17
#define M88E111_REG_PHY_STATUS_FDX (1<<13)
#define M88E111_REG_PHY_STATUS_AUTONEG_RESOLVED (1<<11)
#define M88E111_REG_PHY_STATUS_LINK (1<<10)
#define M88E111_REG_PHY_STATUS_MDIX (1<<6)

#define M88E111_REG_TEST_ENABLE_CTRL M88E111_REG(30, 16)
#define M88E111_REG_TEST_ENABLE_CTRL_CRC (1<<0)

#define M88E111_REG_CRC_RESULT M88E111_REG(30, 12)

#define M88E111_REG_VCT(x) M88E111_REG(28, (x))
#define M88E111_REG_VCT0 M88E111_REG(28, 0)
#define M88E111_REG_VCT0_RUN (1<<15)
#define M88E111_REG_VCT1 M88E111_REG(28, 1)
#define M88E111_REG_VCT2 M88E111_REG(28, 2)
#define M88E111_REG_VCT3 M88E111_REG(28, 3)


uint16_t m88e111_reg_read(uint16_t addr);
void m88e111_reg_write(uint16_t addr, uint16_t data);
void m88e111_reg_sbi(uint16_t addr, uint16_t set_mask);
void m88e111_reg_cbi(uint16_t addr, uint16_t clear_mask);

uint8_t m88e111_check(void);
