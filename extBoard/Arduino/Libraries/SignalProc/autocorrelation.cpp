#include "stdint.h"

/* Calculate the (un-normalized) autocorrelation for a frame of a
signal   */

void autoCorrelation(float *x, float *R, int order)
{

    float sum;
    int i,j;

    for (i=0;i<order;i++) {
        sum=0;
        for (j=0;j<order-i;j++) {
            sum+=x[j]*x[j+i];
        }
        R[i]=sum;
    }
}

void autoCorrelation16(int32_t *x, int32_t *R, int order)
{

    int32_t sum;
    int i,j;

    for (i=0;i<order;i++) {
        sum=0;
        for (j=0;j<order-i;j++) {
            sum+=x[j]*x[j+i];
        }
        R[i]=sum;
    }
}