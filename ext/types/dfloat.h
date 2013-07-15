typedef double dtype;
typedef double rtype;

#include "float_macro.h"

#define m_min_init nary_dfloat_new_dim0(0.0/0.0)
#define m_max_init nary_dfloat_new_dim0(0.0/0.0)
#define m_extract(x) rb_float_new(*(double*)x)
#define m_nearly_eq(x,y) (fabs(x-y)<=(fabs(x)+fabs(y))*DBL_EPSILON*2)
