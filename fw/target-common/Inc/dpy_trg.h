#pragma once
#include "main.h"

void dpy_trg_init(SPI_HandleTypeDef *h);
void dpy_trg_flush(void);
void dpy_trg_enter_sleep(void);
void dpy_trg_exit_sleep(void);
