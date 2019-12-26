#pragma once
#include <stdint.h>

#define VERSION_INFO_SIG 0x1BADB002

typedef struct {
	uint32_t sig;
	uint32_t len;
	char desc[22];
} version_info_t;
