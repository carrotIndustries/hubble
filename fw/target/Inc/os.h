#pragma once
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"
#include "semphr.h"

void os_init(void);

extern QueueHandle_t event_queue;
extern TaskHandle_t sfp_task;
extern SemaphoreHandle_t sfp_mutex;

extern TaskHandle_t bat_task;

void os_enter_sleep(void);
void os_exit_sleep(void);
