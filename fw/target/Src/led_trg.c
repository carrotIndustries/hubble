#include "led_trg.h"

static TIM_HandleTypeDef *htim = 0;

void led_trg_init(TIM_HandleTypeDef *h)
{
    htim = h;
}

static uint16_t led_pwm = 0;

void led_hw_set(uint16_t v)
{
    htim->Instance->CCR1 = v;
    if (led_pwm == 0 && v) {
        HAL_TIM_PWM_Start(htim, TIM_CHANNEL_1);
    }
    else if (led_pwm && v == 0) {
        HAL_TIM_PWM_Stop(htim, TIM_CHANNEL_1);
    }
    led_pwm = v;
}
