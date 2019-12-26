#include "m88e111.h"
#include "m88e111_hw.h"
#include "util.h"

uint8_t m88e111_check(void)
{
    uint16_t r0 = m88e111_reg_read(M88E111_REG_PHHYID0);
    if (r0 != 511) {
        return 0;
    }
    uint16_t r1 = m88e111_reg_read(M88E111_REG_PHHYID1);
    return (((r1 >> 10) & ONES(6)) == 3) && (((r1 >> 4) & ONES(6)) == 0xc);
}


static void set_page(uint8_t page, uint8_t addr)
{
    if (addr == 30) {
        m88e111_hw_reg_write(29, page);
    }
    else {
        m88e111_hw_reg_write(22, page);
    }
}

static void set_page_for_addr(uint16_t addr) {
	set_page(addr>>8, addr&0xff);
}

uint16_t m88e111_reg_read(uint16_t addr)
{
	set_page_for_addr(addr);
    return m88e111_hw_reg_read(addr&0xff);
}

void m88e111_reg_write(uint16_t addr, uint16_t data)
{
	set_page_for_addr(addr);
    m88e111_hw_reg_write(addr&0xff, data);
}

void m88e111_reg_sbi(uint16_t addr, uint16_t set_mask) {
	uint16_t r = m88e111_reg_read(addr);
	r |= set_mask;
	m88e111_reg_write(addr, r);
}

void m88e111_reg_cbi(uint16_t addr, uint16_t clear_mask) {
	uint16_t r = m88e111_reg_read(addr);
	r &= ~clear_mask;
	m88e111_reg_write(addr, r);
}
