typedef float dtype;
typedef float rtype;
#define cT  cSFloat
#define cRT cT
#define mTM mSFloatMath

#include "float_macro.h"

#define m_min_init nary_sfloat_new_dim0(0.0/0.0)
#define m_max_init nary_sfloat_new_dim0(0.0/0.0)

#define m_extract(x) rb_float_new(*(float*)x)
#define m_nearly_eq(x,y) (fabs(x-y)<=(fabs(x)+fabs(y))*FLT_EPSILON*2)
