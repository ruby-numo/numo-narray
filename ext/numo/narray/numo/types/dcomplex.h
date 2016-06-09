typedef dcomplex dtype;
typedef double rtype;
#define cT  numo_cDComplex
#define cRT numo_cDFloat
#define mTM numo_mDComplexMath

#include "complex_macro.h"

static inline bool c_nearly_eq(dtype x, dtype y) {
    return c_abs(c_sub(x,y)) <= (c_abs(x)+c_abs(y))*DBL_EPSILON*2;
}
