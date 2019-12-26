#include "bat_trg.h"
#include "os.h"
#include "common/event.h"
#include "common/bat_hw.h"

static ADC_HandleTypeDef *hadc;

static uint16_t adc_value = 0xff;


void bat_trg_init(ADC_HandleTypeDef *h)
{
    hadc = h;
}

static uint16_t vref_adc = 1;
static const float vref = 1.212;

void bat_task_code(void *param)
{
	if(!hadc) {
		HardFault_Handler();
	}
    vTaskDelay(500 / portTICK_PERIOD_MS);

    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = ADC_CHANNEL_VREFINT;
    sConfig.Rank = ADC_REGULAR_RANK_1;
    sConfig.SamplingTime = ADC_SAMPLETIME_24CYCLES_5;
    sConfig.SingleDiff = ADC_SINGLE_ENDED;
    sConfig.OffsetNumber = ADC_OFFSET_NONE;
    sConfig.Offset = 0;
    if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK) {
        Error_Handler();
    }
    HAL_ADC_Start(hadc);
    HAL_ADC_PollForConversion(hadc, 1000);
    vref_adc = HAL_ADC_GetValue(hadc);

    sConfig.Channel = ADC_CHANNEL_8;
    if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK) {
        Error_Handler();
    }

    while (1) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        HAL_GPIO_WritePin(LEN_VBAT_MEAS_GPIO_Port, LEN_VBAT_MEAS_Pin, GPIO_PIN_RESET);
        vTaskDelay(1 / portTICK_PERIOD_MS);
        HAL_ADC_Start(hadc);
        HAL_ADC_PollForConversion(hadc, 1000);
        float value = HAL_ADC_GetValue(hadc);
        float volt = (value/vref_adc)*2.f*vref;
        adc_value = volt*1000;
        send_event(EVENT_BAT, 0);

        HAL_GPIO_WritePin(LEN_VBAT_MEAS_GPIO_Port, LEN_VBAT_MEAS_Pin, GPIO_PIN_SET);
    }
}

uint16_t bat_hw_read(void)
{
    return adc_value;
}

uint8_t bat_hw_get_state(void) {
	uint8_t state = 0;
	if(HAL_GPIO_ReadPin(CHG_CHG_GPIO_Port, CHG_CHG_Pin) == 0) {
		state |= BAT_STATE_CHG;
	}
	if(HAL_GPIO_ReadPin(CHG_PG_GPIO_Port, CHG_PG_Pin) == 0) {
		state |= BAT_STATE_PG;
	}
	return state;
}
