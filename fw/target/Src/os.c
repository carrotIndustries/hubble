#include "os.h"
#include "sfp_trg.h"
#include "bat_trg.h"
#include "main.h"
#include "common/event.h"
#include "common/util.h"
#include "common/sfp.h"
#include "common/dpy.h"
#include "debounce.h"
#include "timers.h"
#include <string.h>

event_t event_queue_buffer[16];
QueueHandle_t event_queue = 0;
static StaticQueue_t _event_queue;
static StaticTask_t _sfp_task;
TaskHandle_t sfp_task;

SemaphoreHandle_t sfp_mutex;
static StaticSemaphore_t _sfp_mutex;

static StaticTask_t _bat_task;
TaskHandle_t bat_task;

static StaticTimer_t _debounce_timer;
static TimerHandle_t debounce_timer;

static StaticTimer_t _view_timer;
static TimerHandle_t view_timer;

static StaticTimer_t _sleep_timer;
static TimerHandle_t sleep_timer;

StackType_t sfp_task_stack[256];
StackType_t bat_task_stack[256];

static void debounce_timer_cb(TimerHandle_t xTimer)
{
    trg_debounce();
}

static void view_timer_cb(TimerHandle_t xTimer)
{
    event_t ev = {.type = EVENT_TIMER, .param = 0};
    xQueueSend(event_queue, &ev, 0);
}

static void sleep_timer_cb(TimerHandle_t xTimer)
{
    event_t ev = {.type = EVENT_SLEEP_TIMER, .param = 0};
    xQueueSend(event_queue, &ev, 0);
}

void view_timer_start(uint16_t interval_ms)
{
    xTimerChangePeriod(view_timer, interval_ms / portTICK_PERIOD_MS, 1000);
}

void view_timer_stop(void)
{
    xTimerStop(view_timer, 1000);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, signed char *pcTaskName)
{
	dpy_clear();
	dpy_set_font(DPY_FONT_16_BOLD);
    dpy_puts(8, 24, "stack overflow");
    dpy_trg_flush();
    while (1)
        ;
}

void send_event(uint8_t type, uint8_t param)
{
    event_t ev = {.type = type, .param = param};
    xQueueSend(event_queue, &ev, 0);
}

void os_enter_sleep(void) {
	xTimerStop(sleep_timer,0 );
	xTimerStop(debounce_timer,0 );
}

void os_exit_sleep(void) {
	xTimerStart(sleep_timer, 0);
	xTimerStart(debounce_timer, 0);
}

void os_init(void)
{
    event_queue = xQueueCreateStatic(ARRAY_SIZE(event_queue_buffer), sizeof(event_queue_buffer[0]),
                                     (void *)event_queue_buffer, &_event_queue);
    sfp_task = xTaskCreateStatic(sfp_task_code, "sfp_task", ARRAY_SIZE(sfp_task_stack), NULL, 0, sfp_task_stack,
                                 &_sfp_task);
    bat_task = xTaskCreateStatic(bat_task_code, "bat_task", ARRAY_SIZE(bat_task_stack), NULL, 0, bat_task_stack,
                                 &_bat_task);
    sfp_mutex = xSemaphoreCreateMutexStatic(&_sfp_mutex);
    xTaskNotifyGive(sfp_task);

    debounce_timer = xTimerCreateStatic("deb", 10, pdTRUE, 0, debounce_timer_cb, &_debounce_timer);
    xTimerStart(debounce_timer, 0);

    view_timer = xTimerCreateStatic("view", 1000, pdTRUE, 0, view_timer_cb, &_view_timer);
    sleep_timer = xTimerCreateStatic("sleep", 5000, pdTRUE, 0, sleep_timer_cb, &_sleep_timer);
    xTimerStart(sleep_timer, 0);
}
