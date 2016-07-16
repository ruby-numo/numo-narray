typedef dcomplex dtype;
typedef double rtype;
#define cT  numo_cDComplex
#define cRT numo_cDFloat
#define mTM numo_mDComplexMath

#include "complex_macro.h"

static inline bool c_nearly_eq(dtype x, dtype y) {
    return c_abs(c_sub(x,y)) <= (c_abs(x)+c_abs(y))*DBL_EPSILON*2;
}

/* generates a random number on [0,1)-real-interval */
inline static dtype m_rand(dtype max)
{
    dtype z;
    REAL(z) = genrand_res53_mix() * REAL(max);
    IMAG(z) = genrand_res53_mix() * IMAG(max);
    return z;
}
