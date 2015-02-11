#ifndef TIME_H
#define TIME_H

#include "Arduino.h"
#include "stddef.h"

typedef struct TIME {
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint16_t millisec;
	uint16_t microsec;
} TIME;

typedef struct TIMESHORT {
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
	uint16_t millisec;

} TIMESHORT;

typedef struct DATE {
	uint8_t day;
	uint8_t month;
	uint16_t year;
} DATE;
typedef struct DATETIME {
	DATE date;
	TIME time;
} DATETIME;

typedef struct DATETIMESHORT {
	DATE date;
	TIMESHORT time;
} DATETIMESHORT;

typedef uint32_t UNIXTIME;
typedef uint64_t UNIXTIMEMS;

#endif