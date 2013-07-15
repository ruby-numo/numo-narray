typedef float dtype;
typedef float rtype;

#include "float_macro.h"

#define m_min_init nary_sfloat_new_dim0(0.0/0.0)
#define m_max_init nary_sfloat_new_dim0(0.0/0.0)

#define m_extract(x) rb_float_new(*(float*)x)
#define m_nearly_eq(x,y) (fabs(x-y)<=(fabs(x)+fabs(y))*FLT_EPSILON*2)

/*
EXTERN double round(double);
EXTERN double log2(double);
EXTERN double exp2(double);
EXTERN double exp10(double);

#define m_zero 0.0
#define m_one  1.0

#define m_num_to_data(x) NUM2DBL(x)
#define m_data_to_num(x) rb_float_new(x)

#define m_from_double(x) (x)
#define m_from_real(x) (x)

#define m_extract(x) rb_float_new(*(double*)x)

#define m_add(x,y) ((x)+(y))
#define m_sub(x,y) ((x)-(y))
#define m_mul(x,y) ((x)*(y))
#define m_div(x,y) ((x)/(y))
#define m_mod(x,y) fmod(x,y)
#define m_pow(x,y) pow(x,y)

#define m_abs(x)     fabs(x)
#define m_minus(x)   (-(x))
#define m_inverse(x) (1.0/(x))
#define m_square(x)  ((x)*(x))
#define m_floor(x)   floor(x)
#define m_round(x)   round(x)
#define m_ceil(x)    ceil(x)

#define m_eq(x,y) ((x)==(y))
#define m_ne(x,y) ((x)!=(y))
#define m_gt(x,y) ((x)>(y))
#define m_ge(x,y) ((x)>=(y))
#define m_lt(x,y) ((x)<(y))
#define m_le(x,y) ((x)<=(y))

#define m_neary_eq(x,y) (fabs(x-y)<=(fabs(x)+fabs(y))*DBL_EPSILON*2)

#define m_isnan(x) isnan(x)
#define m_isinf(x) isinf(x)
#define m_isfinite(x) isfinite(x)

#define m_sum(x,y) {if (!isnan(x)) {y+=x;}}
#define m_sum_init INT2FIX(0)
#define m_min(x,y) {if (!isnan(x) && (isnan(y) || y>x)) {y=x;}}
#define m_max(x,y) {if (!isnan(x) && (isnan(y) || y<x)) {y=x;}}
#define m_min_init nary_sfloat_new_dim0(0.0/0.0)
#define m_max_init nary_sfloat_new_dim0(0.0/0.0)

#define m_rand to_res53(gen_rand64())
#define m_rand_norm(a) rand_norm(a)

#define m_sprintf(s,x) sprintf(s,"%g",x)

#define cmp(a,b)                                                        \
    (isnan(qsort_cast(a)) ? (isnan(qsort_cast(b)) ? 0 : 1) :            \
     (isnan(qsort_cast(b)) ? -1 :                                       \
      ((qsort_cast(a)==qsort_cast(b)) ? 0 :                             \
       (qsort_cast(a) > qsort_cast(b)) ? 1 : -1)))

#define cmpgt(a,b)                                            \
    ((isnan(qsort_cast(a)) && !isnan(qsort_cast(b))) ||       \
     (qsort_cast(a) > qsort_cast(b)))


#define m_sqrt(x)    sqrt(x)
#define m_cbrt(x)    cbrt(x)
#define m_log(x)     log(x)
#define m_log2(x)    log2(x)
#define m_log10(x)   log10(x)
#define m_exp(x)     exp(x)
#define m_exp2(x)    exp2(x)
#define m_exp10(x)   exp10(x)
#define m_sin(x)     sin(x)
#define m_cos(x)     cos(x)
#define m_tan(x)     tan(x)
#define m_asin(x)    asin(x)
#define m_acos(x)    acos(x)
#define m_atan(x)    atan(x)
#define m_sinh(x)    sinh(x)
#define m_cosh(x)    cosh(x)
#define m_tanh(x)    tanh(x)
#define m_asinh(x)   asinh(x)
#define m_acosh(x)   acosh(x)
#define m_atanh(x)   atanh(x)
#define m_atan2(x,y) atan2(x,y)
#define m_hypot(x,y) hypot(x,y)

#define m_erf(x)     erf(x)
#define m_erfc(x)    erfc(x)
#define m_ldexp(x,y) ldexp(x,y)
#define m_frexp(x,exp) frexp(x,exp)
*/
