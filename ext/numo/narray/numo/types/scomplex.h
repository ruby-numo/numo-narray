typedef scomplex dtype;
typedef float rtype;
#define cT  numo_cSComplex
#define cRT numo_cSFloat
#define mTM numo_mSComplexMath

#include "complex_macro.h"

static inline bool c_nearly_eq(dtype x, dtype y) {
    return c_abs(c_sub(x,y)) <= (c_abs(x)+c_abs(y))*FLT_EPSILON*2;
}
