typedef scomplex dtype;
typedef float rtype;
#define cT  numo_cSComplex
#define cRT numo_cSFloat
#define mTM numo_mSComplexMath

#include "complex_macro.h"

static inline bool c_nearly_eq(dtype x, dtype y) {
    return c_abs(c_sub(x,y)) <= (c_abs(x)+c_abs(y))*FLT_EPSILON*2;
}

/* generates a random number on [0,1)-real-interval */
inline static dtype m_rand(dtype max)
{
    dtype z;
    REAL(z) = to_real2(gen_rand32()) * REAL(max);
    IMAG(z) = to_real2(gen_rand32()) * IMAG(max);
    return z;
}
