#pragma once
#include <stdint.h>

typedef enum {
    SFP_STATE_NOT_PRESENT,
    SFP_STATE_PRESENT,
    SFP_STATE_READY,
    SFP_STATE_ERROR
} sfp_state_t;

#define SFP_I2C_ADDR_SID 0xa0
#define SFP_I2C_ADDR_MON 0xa2
#define SFP_I2C_ADDR_88E111 0xac

uint8_t sfp_state_get(void);
void sfp_state_set(uint8_t state);

typedef enum {
	SFP_MODEL_GENERIC,
	SFP_MODEL_88E111
} sfp_model_t;

uint8_t sfp_model_get(void);
void sfp_model_set(uint8_t state);

typedef struct __attribute__((packed)) {
        uint8_t id;
        uint8_t ext_id;
        uint8_t connector;
        uint8_t transceiver[8];
        uint8_t encoding;
        uint8_t baudrate;
        uint8_t rate;
        uint8_t length_sm_km;
        uint8_t length_sm_100m;
        uint8_t length_om2;
        uint8_t length_om1;
        uint8_t length_copper;
        uint8_t length_om3;
        char vendor_name[16];
        uint8_t transceiver2;
        uint8_t vendor_oui[3];
        char vendor_pn[16];
        uint8_t vendor_rev[4];
        uint16_t wavelength;
        uint8_t unallocated1;
        uint8_t cc_base;
        uint8_t options[2];
        uint8_t upper_bit_rate_margin;
        uint8_t lower_bit_rate_margin;
        char vendor_sn[16];
        uint8_t date_code[8];
        uint8_t dm_type;
        uint8_t enhanced_options;
        uint8_t sff8472_compliance;
        uint8_t cc_ext;
} sfp_sid_t;


_Static_assert (sizeof(sfp_sid_t)==96, "incorrect length"); /* { dg-error "static assertion failed: \"assert1\"" } */

typedef struct __attribute__((packed)) {
    uint8_t integ;
    uint8_t frac;
} sfp_fix_t;

typedef struct __attribute__((packed)) {
        /* A/W Thresholds, Table 3.15 */
        uint16_t temp_high_alarm;
        uint16_t temp_low_alarm;
        uint16_t temp_high_warning;
        uint16_t temp_low_warning;
        uint16_t voltage_high_alarm;
        uint16_t voltage_low_alarm;
        uint16_t voltage_high_warning;
        uint16_t voltage_low_warning;
        uint16_t bias_high_alarm;
        uint16_t bias_low_alarm;
        uint16_t bias_high_warning;
        uint16_t bias_low_warning;
        uint16_t tx_power_high_alarm;
        uint16_t tx_power_low_alarm;
        uint16_t tx_power_high_warning;
        uint16_t tx_power_low_warning;
        uint16_t rx_power_high_alarm;
        uint16_t rx_power_low_alarm;
        uint16_t rx_power_high_warning;
        uint16_t rx_power_low_warning;
        uint8_t unallocated1[16];

        /* Ext Cal Constants, Table 3.16 */
        float rx_pwr4;
        float rx_pwr3;
        float rx_pwr2;
        float rx_pwr1;
        float rx_pwr0;
        sfp_fix_t tx_i_slope;
        uint16_t tx_i_offset;
        sfp_fix_t tx_pwr_slope;
        int16_t tx_pwr_offset;
        sfp_fix_t t_slope;
        int16_t t_offset;
        sfp_fix_t v_slope;
        uint16_t v_offset;
        uint8_t unallocated2[3];
        uint8_t calibration_checksum;
} sfp_alarm_cal_t;

_Static_assert (sizeof(sfp_alarm_cal_t)==96, "incorrect length"); /* { dg-error "static assertion failed: \"assert1\"" } */

typedef struct __attribute__((packed)) {
        uint16_t temp;
        uint16_t vcc;
        uint16_t tx_bias;
        uint16_t tx_power;
        uint16_t rx_power;
        uint8_t unallocated3[4];
        uint8_t status_control;
        uint8_t reserved1;

        /* Diagnostic Alarm/Warning Flags Status Bits, Table 3.18 */
        uint8_t alarm_flags[2];
        uint8_t unallocated4[2];
        uint8_t warning_flags[2];

        /* Extended Status/Control, Table 3.18a */
        uint8_t ext_status_control[2];
} sfp_mon_t;

_Static_assert (sizeof(sfp_mon_t)==24, "incorrect length"); /* { dg-error "static assertion failed: \"assert1\"" } */

sfp_sid_t  *sfp_sid_get(void);
sfp_alarm_cal_t  *sfp_alarm_cal_get(void);
sfp_mon_t  *sfp_mon_get(void);

typedef struct {
	uint8_t addrs[6];
} sfp_i2c_scan_t;

sfp_i2c_scan_t *sfp_i2c_scan_get(void);
void sfp_i2c_scan_reset(void);
uint8_t sfp_i2c_scan_has_addr8(uint8_t addr);

uint16_t sfp_get_rx_power(void);
int16_t sfp_uw_to_dBm(uint16_t uw);
