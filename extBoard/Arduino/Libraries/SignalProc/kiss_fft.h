#ifndef KISS_FFT_H
#define KISS_FFT_H

#ifndef PACKED
#define PACKED __attribute__ ((packed))
#endif

#define	__need_NULL
#define	__need_size_t
#include <stddef.h>

#include "math.h"

#include <inttypes.h>
/********************************************************
 * definitions
 *******************************************************/
#define FIXED_POINT 32
 
#ifdef FIXED_POINT
# if (FIXED_POINT == 32)
#  define kiss_fft_scalar int32_t
# else	
#  define kiss_fft_scalar int16_t
# endif
#else
# ifndef kiss_fft_scalar
/*  default is float */
#   define kiss_fft_scalar float
# endif
#endif

#define MAXFACTORS 32

/******************************************************************
 * types
 *****************************************************************/

typedef struct PACKED {
    kiss_fft_scalar r;
    kiss_fft_scalar i;
}kiss_fft_cpx;

struct kiss_fft_state{
    int nfft;
    int inverse;
    int factors[2*MAXFACTORS];
    kiss_fft_cpx twiddles[1];
};

typedef struct kiss_fft_state* kiss_fft_cfg;

typedef struct kiss_fftr_state *kiss_fftr_cfg;

struct kiss_fftr_state{
    kiss_fft_cfg substate;
    kiss_fft_cpx * tmpbuf;
    kiss_fft_cpx * super_twiddles;
};

/********************************************************
 * definitions
 *******************************************************/

#ifdef FIXED_POINT
#if (FIXED_POINT==32)
# define FRACBITS 31
# define SAMPPROD int64_t
#define SAMP_MAX 2147483647
#else
# define FRACBITS 15
# define SAMPPROD int32_t 
#define SAMP_MAX 32767
#endif

#define SAMP_MIN -SAMP_MAX

#if defined(CHECK_OVERFLOW)
#  define CHECK_OVERFLOW_OP(a,op,b)  \
	if ( (SAMPPROD)(a) op (SAMPPROD)(b) > SAMP_MAX || (SAMPPROD)(a) op (SAMPPROD)(b) < SAMP_MIN ) { \
		fprintf(stderr,"WARNING:overflow @ " __FILE__ "(%d): (%d " #op" %d) = %ld\n",__LINE__,(a),(b),(SAMPPROD)(a) op (SAMPPROD)(b) );  }
#endif


#   define smul(a,b) ( (SAMPPROD)(a)*(b) )

#   define sround( x )  (kiss_fft_scalar)( ( (x) + ( ((kiss_fft_scalar)1)<<(FRACBITS-1)) ) >> FRACBITS )

/* function can reduce code size, but a little more execution time (less than 10ms on TelosB */
#if defined(USE_ARITH_FUNC)
kiss_fft_scalar S_MUL(kiss_fft_scalar a, kiss_fft_scalar b) {
     return sround( smul(a, b) );
};
#else
#   define S_MUL(a,b) sround( smul(a,b) )
#endif

#if defined(USE_ARITH_FUNC)
#define C_MUL(m,a,b) C_MUL_wrapper(&(m), &(a), &(b));
void C_MUL_wrapper(kiss_fft_cpx *m, kiss_fft_cpx *a, kiss_fft_cpx *b)
{
     m->r = sround( smul(a->r,b->r) - smul(a->i,b->i) );
     m->i = sround( smul(a->r,b->i) + smul(a->i,b->r) );
}
#else
#   define C_MUL(m,a,b) \
      do{ (m).r = sround( smul((a).r,(b).r) - smul((a).i,(b).i) ); \
          (m).i = sround( smul((a).r,(b).i) + smul((a).i,(b).r) ); }while(0)
#endif



#   define DIVSCALAR(x,k) \
	(x) = sround( smul(  x, SAMP_MAX/k ) )

/* the following C_FIXDIV(c,div) is replaced by null operation
 * first, it reduces code size
 * second, the result will be same as the float version
 */
/*
#   define C_FIXDIV(c,div) \
	do {    DIVSCALAR( (c).r , div);  \
		DIVSCALAR( (c).i  , div); }while (0)
*/
#define C_FIXDIV(c,div)


#   define C_MULBYSCALAR( c, s ) \
    do{ (c).r =  sround( smul( (c).r , s ) ) ;\
        (c).i =  sround( smul( (c).i , s ) ) ; }while(0)

#else  /* not FIXED_POINT*/

#   define S_MUL(a,b) ( (a)*(b) )
#define C_MUL(m,a,b) \
    do{ (m).r = (a).r*(b).r - (a).i*(b).i;\
        (m).i = (a).r*(b).i + (a).i*(b).r; }while(0)
#   define C_FIXDIV(c,div) /* NOOP */
#   define C_MULBYSCALAR( c, s ) \
    do{ (c).r *= (s);\
        (c).i *= (s); }while(0)
#endif

#ifndef CHECK_OVERFLOW_OP
#  define CHECK_OVERFLOW_OP(a,op,b) /* noop */
#endif

#define  C_ADD( res, a,b)\
    do { \
	    CHECK_OVERFLOW_OP((a).r,+,(b).r)\
	    CHECK_OVERFLOW_OP((a).i,+,(b).i)\
	    (res).r=(a).r+(b).r;  (res).i=(a).i+(b).i; \
    }while(0)
#define  C_SUB( res, a,b)\
    do { \
	    CHECK_OVERFLOW_OP((a).r,-,(b).r)\
	    CHECK_OVERFLOW_OP((a).i,-,(b).i)\
	    (res).r=(a).r-(b).r;  (res).i=(a).i-(b).i; \
    }while(0)
#define C_ADDTO( res , a)\
    do { \
	    CHECK_OVERFLOW_OP((res).r,+,(a).r)\
	    CHECK_OVERFLOW_OP((res).i,+,(a).i)\
	    (res).r += (a).r;  (res).i += (a).i;\
    }while(0)

#define C_SUBFROM( res , a)\
    do {\
	    CHECK_OVERFLOW_OP((res).r,-,(a).r)\
	    CHECK_OVERFLOW_OP((res).i,-,(a).i)\
	    (res).r -= (a).r;  (res).i -= (a).i; \
    }while(0)


#ifdef FIXED_POINT
#  define KISS_FFT_COS(phase)  floor(.5+SAMP_MAX * cos (phase))
#  define KISS_FFT_SIN(phase)  floor(.5+SAMP_MAX * sin (phase))
#  define HALF_OF(x) ((x)>>1)
#elif defined(USE_SIMD)
#  define KISS_FFT_COS(phase) _mm_set1_ps( cos(phase) )
#  define KISS_FFT_SIN(phase) _mm_set1_ps( sin(phase) )
#  define HALF_OF(x) ((x)*_mm_set1_ps(.5))
#else
#  define KISS_FFT_COS(phase) (kiss_fft_scalar) cos(phase)
#  define KISS_FFT_SIN(phase) (kiss_fft_scalar) sin(phase)
#  define HALF_OF(x) ((x)*.5)
#endif

#define  kf_cexp(x,phase) \
	do{ \
		(x)->r = KISS_FFT_COS(phase);\
		(x)->i = KISS_FFT_SIN(phase);\
	}while(0)



/****************************************************************
 * exported functions
 ***************************************************************/
size_t kiss_fftr_mem(int nfft);

kiss_fftr_cfg kiss_fftr_alloc(int nfft,int inverse_fft,void * mem, size_t * lenmem);
void kiss_fftr(kiss_fftr_cfg cfg, kiss_fft_scalar *timedata,kiss_fft_cpx *freqdata);

#endif
