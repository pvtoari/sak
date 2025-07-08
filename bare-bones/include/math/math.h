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

/*
double pow(double a, double b) {
    double res = 1;
    
    while (b > 0) {
        if (b & 1) res = res * a;
        a = a * a;
        b >>= 1;
    }
    
    return res;
}
*/

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

#endif // MATH_H
