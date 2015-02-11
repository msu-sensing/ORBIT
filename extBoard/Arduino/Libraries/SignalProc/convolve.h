#ifndef _CONVOLVE_H
#define _CONVOLVE_H

#include "Arduino.h"
#include "stddef.h"

void convolve(int32_t Signal[/* SignalLen */], int SignalLen,
              int32_t Kernel[/* KernelLen */], int KernelLen,
              int32_t Result[/* SignalLen + KernelLen - 1 */]);
			  
#endif