#include <stdint.h>

#include "math/costable_0_001.h"
#include "math/ieee_float.h"
#include "std/errno.h"

#ifndef MATH_H
#define MATH_H

#define FORCE_EVAL(x) do { \
    volatile double __x = (x); \
    (void)__x; \
} while(0)

// E approx 1e-21
#define PI 3.14159265358979323846
#define PI_2 1.57079632679489661923
#define TAU 6.28318530717958647692

// https://en.cppreference.com/w/cpp/numeric/math/HUGE_VAL
#define HUGE_VALF  3.402823466e+38F
#define HUGE_VAL   1.7976931348623158e+308
#define HUGE_VALL  1.189731495357231765e+4932L  // según plataforma


// https://www.netlib.org/fdlibm/fdlibm.h
#if defined(i386) || defined(i486) || \
	defined(intel) || defined(x86) || defined(i86pc) || \
	defined(__alpha) || defined(__osf__)
#define __LITTLE_ENDIAN
#endif

#ifdef __LITTLE_ENDIAN
#define __HI(x) *(1+(int*)&x)
#define __LO(x) *(int*)&x
#define __HIp(x) *(1+(int*)x)
#define __LOp(x) *(int*)x
#else
#define __HI(x) *(int*)&x
#define __LO(x) *(1+(int*)&x)
#define __HIp(x) *(int*)x
#define __LOp(x) *(1+(int*)x)
#endif

#ifdef __STDC__
static const double huge = 1.0e300;
#define	__P(p)	p
#else
static double huge = 1.0e300;
#define	__P(p)	()
#endif

static inline int min(int a, int b) {
    return (b < a) ? b : a;
}

static inline int max(int a, int b) {
    return (b < a) ? a : b;
}

static inline double absd(double x) {
    return x < 0 ? -x : x;
}

static inline int abs(int x) {
    return x < 0 ? -x : x;
}

double trunc(double x) {
	union {double f; uint64_t i;} u = {x};
	int e = (int)(u.i >> 52 & 0x7ff) - 0x3ff + 12;
	uint64_t m;

	if (e >= 52 + 12) return x;
	if (e < 12) e = 1;
	
	m = -1ULL >> e;
	if ((u.i & m) == 0) return x;

	FORCE_EVAL(x + 0x1p120f);
	u.i &= ~m;

	return u.f;
}

double fmod(double x, double y) {
    return x - trunc(x / y) * y;
}

static inline double radnorm(double x) {
    double r = fmod(x, TAU);
    
    return r < 0 ? r + TAU : r;
}

static inline double lerp(double w, double v1, double v2) {
    return (1.0 - w) * v1 + w * v2;
}

double cos(double x) {
    x = radnorm(x);

    double index = x / COS_TABLE_STEP;
    int i1 = (int) index, i2 = (i1 + 1) % COS_TABLE_SIZE;
    
    return lerp(index-i1, costable[i1], costable[i2]);
}

double sin(double x) {
    return cos(x - PI_2);
}

double pow(double x, double n) {
    if(x==0 || x==1) return x;
    
    double ans = 1; 
    if(n < 0) {
        x = 1/x;
        n = -(n+1);
        ans = ans * x;
    }
    
    while(n>0) {
        if((int) n % 2 == 1) {
            ans = ans * x;
            n = n - 1;
        } else {
            n = n/2;
            x = x * x;
        }
    }

    return ans;
}

double ldexp(double value, int exp) {
	struct f64 *f64p;
	int oldexp, exp_bias;
	double factor;

	f64p= (struct f64 *)&value;
	exp_bias= 0;

	oldexp= F64_GET_EXP(f64p);
	if (oldexp == F64_EXP_MAX) {	
		/* Either infinity or Nan */
		return value;
	}

	if (oldexp == 0) {
		/* Either 0 or denormal */
		if (F64_GET_MANT_LOW(f64p) == 0 &&
			F64_GET_MANT_HIGH(f64p) == 0) {
			return value;
		}
	}

	/* If exp is too large (> 2*F64_EXP_MAX) or too small
	 * (< -2*F64_EXP_MAX) return HUGE_VAL or 0. This prevents overflows
	 * in exp if exp is really weird
	 */
	if (exp >= 2*F64_EXP_MAX) {
		errno= ERANGE;
		return HUGE_VAL;
	}

	if (exp <= -2*F64_EXP_MAX) {
		errno= ERANGE;
		return 0;
	}
	
	/* Normalize a denormal */
	if (oldexp == 0) {
		/* Multiply by 2^64 */
		factor= 65536.0;	/* 2^16 */
		factor *= factor;	/* 2^32 */
		factor *= factor;	/* 2^64 */
		value *= factor;
		exp= -64;
		oldexp= F64_GET_EXP(f64p);
	}

	exp= oldexp + exp;
	if (exp >= F64_EXP_MAX) {
		/* Overflow */
		 errno= ERANGE;
		return HUGE_VAL;
	}

	if (exp > 0) {
		/* Normal */
		F64_SET_EXP(f64p, exp);
		return value;
	}

	/* Denormal, or underflow. */
	exp += 64;
	F64_SET_EXP(f64p, exp);
	/* Divide by 2^64 */
	factor= 65536.0;	/* 2^16 */
	factor *= factor;	/* 2^32 */
	factor *= factor;	/* 2^64 */
	value /= factor;
	if (value == 0.0) {
		/* Underflow */
		errno= ERANGE;
	}

	return value;
}

double ceil(double x) {
	int i0,i1,j0;
	unsigned i,j;
	i0 =  __HI(x);
	i1 =  __LO(x);
	j0 = ((i0>>20)&0x7ff)-0x3ff;
	if(j0<20) {
	    if(j0<0) { 	/* raise inexact if x != 0 */
		if(huge+x>0.0) {/* return 0*sign(x) if |x|<1 */
		    if(i0<0) {i0=0x80000000;i1=0;} 
		    else if((i0|i1)!=0) { i0=0x3ff00000;i1=0;}
		}
	    } else {
		i = (0x000fffff)>>j0;
		if(((i0&i)|i1)==0) return x; /* x is integral */
		if(huge+x>0.0) {	/* raise inexact flag */
		    if(i0>0) i0 += (0x00100000)>>j0;
		    i0 &= (~i); i1=0;
		}
	    }
	} else if (j0>51) {
	    if(j0==0x400) return x+x;	/* inf or NaN */
	    else return x;		/* x is integral */
	} else {
	    i = ((unsigned)(0xffffffff))>>(j0-20);
	    if((i1&i)==0) return x;	/* x is integral */
	    if(huge+x>0.0) { 		/* raise inexact flag */
		if(i0>0) {
		    if(j0==20) i0+=1; 
		    else {
			j = i1 + (1<<(52-j0));
			if(j<i1) i0+=1;	/* got a carry */
			i1 = j;
		    }
		}
		i1 &= (~i);
	    }
	}
	__HI(x) = i0;
	__LO(x) = i1;
	return x;
}

double floor(double x) {
	int i0,i1,j0;
	unsigned i,j;
	i0 =  __HI(x);
	i1 =  __LO(x);
	j0 = ((i0>>20)&0x7ff)-0x3ff;
	if(j0<20) {
	    if(j0<0) { 	/* raise inexact if x != 0 */
		if(huge+x>0.0) {/* return 0*sign(x) if |x|<1 */
		    if(i0>=0) {i0=i1=0;} 
		    else if(((i0&0x7fffffff)|i1)!=0)
			{ i0=0xbff00000;i1=0;}
		}
	    } else {
		i = (0x000fffff)>>j0;
		if(((i0&i)|i1)==0) return x; /* x is integral */
		if(huge+x>0.0) {	/* raise inexact flag */
		    if(i0<0) i0 += (0x00100000)>>j0;
		    i0 &= (~i); i1=0;
		}
	    }
	} else if (j0>51) {
	    if(j0==0x400) return x+x;	/* inf or NaN */
	    else return x;		/* x is integral */
	} else {
	    i = ((unsigned)(0xffffffff))>>(j0-20);
	    if((i1&i)==0) return x;	/* x is integral */
	    if(huge+x>0.0) { 		/* raise inexact flag */
		if(i0<0) {
		    if(j0==20) i0+=1; 
		    else {
			j = i1+(1<<(52-j0));
			if(j<i1) i0 +=1 ; 	/* got a carry */
			i1=j;
		    }
		}
		i1 &= (~i);
	    }
	}
	__HI(x) = i0;
	__LO(x) = i1;
	return x;
}

#endif // MATH_H
