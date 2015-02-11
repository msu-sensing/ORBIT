#ifndef SAMPLE_H
#define SAMPLE_H

#include <time.h>
#ifndef PACKED
#define PACKED __attribute__ ((packed))
#endif

#define MAX_READINGS 1

typedef struct PACKED {
	UNIXTIMEMS tm;
	int32_t reading[MAX_READINGS];
} SAMPLE;

#endif