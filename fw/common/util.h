#pragma once

#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))
#define ARRAY_SIZE(arr)		(sizeof (arr) / sizeof ((arr)[0]))
#define CLAMP(x, low, high)  (((x) > (high)) ? (high) : (((x) < (low)) ? (low) : (x)))
#define CLAMP_ABS(x, lim) (CLAMP((x), (-lim), (lim)))
#define ONES(n) ((1 << (n)) - 1)
#define ABS(a)	   (((a) < 0) ? -(a) : (a))
#define INC_MOD(x, mod) do { \
	if((x) == (mod)-1) { \
		(x) = 0; \
	} \
	else { \
		(x)++; \
	} \
	} while (0)

#define DEC_MOD(x, mod) do { \
	if((x) == 0) { \
		(x) = (mod)-1; \
	} \
	else { \
		(x)--; \
	} \
	} while (0)
