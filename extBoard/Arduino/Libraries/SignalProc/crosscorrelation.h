#ifndef _CROSSCORRELATION_H
#define _CROSSCORRELATION_H

#include "Arduino.h"
#include "stddef.h"
void crossCorrelation(int32_t *x, int32_t *y, int32_t *r, int n, int maxdelay);

#endif