typedef scomplex dtype;
typedef float rtype;
#define cT  cSComplex
#define cRT cSFloat
#define mTM mSComplexMath

#include "complex_macro.h"

static inline boolean c_nearly_eq(dtype x, dtype y) {
    return c_abs(c_sub(x,y)) <= (c_abs(x)+c_abs(y))*FLT_EPSILON*2;
}
