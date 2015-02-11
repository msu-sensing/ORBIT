/*
* This file contains the support needed to perform fixed point math
*
*/
#ifndef _FIXMATH_H
#define _FIXMATH_H

#include "stdint.h"
#include "stdlib.h"
#include "string.h"
#include "math.h"

#define	QN32 16
#define	QN16 8

#define FLTOFIX32(A) (int32_t)((A)*((int32_t)2<<(QN32-1))) 
#define FLTOFIX16(A) (int16_t)((A)*((int32_t)2<<(QN16-1))) 

#define FIXMUL32(A,B) (int32_t)((((int64_t)A)*((int64_t)B))/((int32_t)2<<(QN32-1)))
#define	FIXDIV32(A,B) (int32_t)(((int64_t)(A)/((int32_t)2<<(QN32-1)))/(B))
#define FIXMUL16(A,B) (int16_t)((((int32_t)A)*((int32_t)B))/((int32_t)2<<(QN16-1)))
#define	FIXDIV16(A,B) (int16_t)(((int32_t)(A)/((int32_t)2<<(QN16-1)))/(B))

/*
*	Statistics Routines
*/
uint16_t isqrt(uint32_t value);
int32_t	mean(int32_t * buffer, uint16_t bufferlen);
int32_t	var(int32_t	*buffer, uint16_t bufferlen);
int32_t ABS_int32(int32_t value);

int32_t	mean16(int16_t *buffer, uint16_t bufferlen);
int32_t	var16(int16_t *buffer, uint16_t bufferlen);
int16_t ABS_int16(int16_t value);

#endif