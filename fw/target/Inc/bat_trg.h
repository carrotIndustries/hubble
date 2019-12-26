#pragma once
#include "main.h"
#include "common/sfp_hw.h"

void bat_trg_init(ADC_HandleTypeDef *h);

void bat_task_code(void *param);
