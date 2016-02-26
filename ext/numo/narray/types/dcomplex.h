typedef dcomplex dtype;
typedef double rtype;
#define cT  cDComplex
#define cRT cDFloat
#define mTM mDComplexMath

#include "complex_macro.h"

static inline boolean c_nearly_eq(dtype x, dtype y) {
    return c_abs(c_sub(x,y)) <= (c_abs(x)+c_abs(y))*DBL_EPSILON*2;
}
