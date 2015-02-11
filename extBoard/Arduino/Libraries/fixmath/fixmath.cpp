/*
* These routines implement simple statistics functions
*	mean
*	variance
*	absolute value
*
*/
#include "fixmath.h"
//++
//
//  NAME: isqrt
//
//  FUNCTIONAL DESCRIPTION:
//      returns the unsigned 16 bit square root of a 32 bit quantity
//
//  FORMAL PARAMETERS:
//      val - unsigned 32 bit input value
//
//  IMPLICIT INPUTS:
//      None
//  IMPLICIT OUTPUTS:
//      None
//  RETURN VALUES:
//      16 bit unsigned result
//  SIDE EFFECTS:
//      None
//--
uint16_t isqrt(uint32_t val)
{
    uint32_t temp, g=0, tt;

    if (val >= 0x40000000) {
        g = 0x8000; 
        val -= 0x40000000;
    }

#define INNER_ISQRT(s)                       \
tt = 1;                                      \
temp = (g << (s)) + (tt << ((s) * 2 - 2));   \
if (val >= temp) {                           \
g = g + (1 << ((s)-1));                      \
val = val - temp;                            \
}

    INNER_ISQRT (15)
    INNER_ISQRT (14)
    INNER_ISQRT (13)
    INNER_ISQRT (12)
    INNER_ISQRT (11)
    INNER_ISQRT (10)
    INNER_ISQRT ( 9)
    INNER_ISQRT ( 8)
    INNER_ISQRT ( 7)
    INNER_ISQRT ( 6)
    INNER_ISQRT ( 5)
    INNER_ISQRT ( 4)
    INNER_ISQRT ( 3)
    INNER_ISQRT ( 2)

#undef INNER_ISQRT

    temp = g+g+1;
    if (val >= temp) g++;
    return g;
}

int32_t	mean(int32_t * buf, uint16_t len)
{
	uint16_t i;
	int32_t	sum=0;
	for (i=0;i<len;i++)
	{
		sum+=(int32_t)buf[i];

	}
	return(sum/len);
}

int32_t	var(int32_t	* buf, uint16_t	len)
	{
		uint16_t	i;
		int32_t	mean_val=mean(buf,len);
		int32_t	int_part;
		int32_t	total_power=0;
		for (i=0;i<len;i++)
		{
			int_part=(buf[i]-mean_val);
			total_power+=((int32_t)int_part*(int32_t)int_part)>>QN32; ///((int32_t)2<<(QN32-1));
		}
		return total_power;
	}


int32_t ABS_int32(int32_t val)
{
	if (val<0)
		val=-val;
	return val;
}

int32_t	mean16(int16_t * buf, uint16_t len)
{
	uint16_t i;
	int32_t	sum=0;
	for (i=0;i<len;i++)
	{
		sum+=(int32_t)buf[i];

	}
	return(sum/len);
}

int32_t	var16(int16_t * buf, uint16_t	len)
	{
		uint16_t	i;
		int32_t	mean_val=mean16(buf,len);
		int32_t	int_part;
		int32_t	total_power=0;
		for (i=0;i<len;i++)
		{
			int_part=(buf[i]-mean_val);
			total_power+=((int32_t)int_part*(int32_t)int_part)>>QN32; ///((int32_t)2<<(QN32-1));
		}
		return total_power;
	}


int16_t ABS_int16(int16_t val)
{
	if (val<0)
		val=-val;
	return val;
}

