#include "sfp.h"
#include "util.h"
#include <math.h>
#include <string.h>

static uint32_t sfp_state = SFP_STATE_NOT_PRESENT;

void sfp_state_set(uint8_t state)
{
    sfp_state = state;
}

uint8_t sfp_state_get(void)
{
    return sfp_state;
}

static sfp_sid_t sfp_sid;

sfp_sid_t *sfp_sid_get(void)
{
    return &sfp_sid;
}

static sfp_alarm_cal_t sfp_alarm_cal;

sfp_alarm_cal_t *sfp_alarm_cal_get(void)
{
    return &sfp_alarm_cal;
}

static sfp_mon_t sfp_mon;

sfp_mon_t *sfp_mon_get(void)
{
    return &sfp_mon;
}

static sfp_i2c_scan_t sfp_i2c_scan_result;

sfp_i2c_scan_t *sfp_i2c_scan_get(void)
{
    return &sfp_i2c_scan_result;
}

void sfp_i2c_scan_reset(void)
{
    memset(sfp_i2c_scan_result.addrs, 0xff, ARRAY_SIZE(sfp_i2c_scan_result.addrs));
}

uint8_t sfp_i2c_scan_has_addr8(uint8_t addr)
{
    for (uint8_t i = 0; i < ARRAY_SIZE(sfp_i2c_scan_result.addrs); i++) {
        if (addr == sfp_i2c_scan_result.addrs[i] << 1)
            return 1;
    }
    return 0;
}

uint16_t sfp_get_rx_power(void)
{
    if (sfp_sid.dm_type & (1 << 4)) { // externally calibrated
        uint16_t r = sfp_mon.rx_power;
        return roundf(sfp_alarm_cal.rx_pwr4 * r * r * r * r + sfp_alarm_cal.rx_pwr3 * r * r * r
                      + sfp_alarm_cal.rx_pwr2 * r * r + sfp_alarm_cal.rx_pwr1 * r + sfp_alarm_cal.rx_pwr0);
    }
    else { // must be interally calibrated
        return sfp_mon.rx_power;
    }
}


static const float a[] = { -0.5527074855 };
static const float b[] = { -0.6632718214e+1 };
static const float C1 = 0.693145752;
static const float C2 = 1.428606820e-06;
static const float C3 = 0.4342944819;

static float
logarithmf (float x,
        int ten)
{
  int N;
  float f, w, z;

  /* Check for domain/range errors here. */
  if (x == 0.0)
    {
      return -INFINITY;
    }
  else if (x < 0.0)
    {
      return NAN;
    }
  else if (!isfinite(x))
    {
      if (isnanf(x))
        return NAN;
      else
        return INFINITY;
    }

  /* Get the exponent and mantissa where x = f * 2^N. */
  f = frexpf (x, &N);

  z = f - 0.5;

  if (f > M_SQRT1_2)
    z = (z - 0.5) / (f * 0.5 + 0.5);
  else
    {
      N--;
      z /= (z * 0.5 + 0.5);
    }
  w = z * z;

  /* Use Newton's method with 4 terms. */
  z += z * w * (a[0]) / ((w + 1.0) * w + b[0]);

  if (N != 0)
    z = (N * C2 + z) + N * C1;

  if (ten)
    z *= C3;

  return (z);
}

int16_t sfp_uw_to_dBm(uint16_t uw) //0.1uW
{
	if(uw == 0)
		return -990;
    return roundf(100.f * logarithmf(uw, 10) - 400.f);
}

static uint8_t sfp_model = SFP_MODEL_GENERIC;

uint8_t sfp_model_get(void) {
	return sfp_model;
}

void sfp_model_set(uint8_t m) {
	sfp_model = m;
}
