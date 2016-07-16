typedef float dtype;
typedef float rtype;
#define cT  numo_cSFloat
#define cRT cT
#define mTM numo_mSFloatMath

#include "float_macro.h"

/* generates a random number on [0,1)-real-interval */
inline static dtype m_rand(dtype max)
{
    return to_real2(gen_rand32()) * max;
}

#define m_min_init numo_sfloat_new_dim0(0.0/0.0)
#define m_max_init numo_sfloat_new_dim0(0.0/0.0)

#define m_extract(x) rb_float_new(*(float*)x)
#define m_nearly_eq(x,y) (fabs(x-y)<=(fabs(x)+fabs(y))*FLT_EPSILON*2)
