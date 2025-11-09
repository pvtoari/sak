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

static inline double lerp(double w, double v1, double v2) {
    return (1.0 - w) * v1 + w * v2;
}

double trunc(double x);

double fmod(double x, double y);

static inline double radnorm(double x) {
    double r = fmod(x, TAU);
    
    return r < 0 ? r + TAU : r;
}

double cos(double x);

double sin(double x);

double pow(double x, double n);

double ldexp(double value, int exp);

double ceil(double x);

double floor(double x);

#endif // MATH_H
