typedef double dtype;
typedef double rtype;
#define cT  numo_cDFloat
#define cRT numo_cDFloat
#define mTM numo_mDFloatMath

#include "float_macro.h"

/* generates a random number on [0,1)-real-interval */
inline static dtype m_rand(dtype max)
{
    return to_res53(gen_rand64()) * max;
}

#define m_min_init numo_dfloat_new_dim0(0.0/0.0)
#define m_max_init numo_dfloat_new_dim0(0.0/0.0)
#define m_extract(x) rb_float_new(*(double*)x)
#define m_nearly_eq(x,y) (fabs(x-y)<=(fabs(x)+fabs(y))*DBL_EPSILON*2)
