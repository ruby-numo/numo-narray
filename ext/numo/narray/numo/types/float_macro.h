#include "float_def.h"

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

#define m_add(x,y) ((x)+(y))
#define m_sub(x,y) ((x)-(y))
#define m_mul(x,y) ((x)*(y))
#define m_div(x,y) ((x)/(y))
#define m_div_check(x,y) ((y)==0)
#define m_mod(x,y) fmod(x,y)
#define m_divmod(x,y,a,b) {a=(x)/(y); b=m_mod(x,y);}
#define m_pow(x,y) pow(x,y)
#define m_pow_int(x,y) pow_int(x,y)

#define m_abs(x)     fabs(x)
#define m_minus(x)   (-(x))
#define m_inverse(x) (1/(x))
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

#define m_isnan(x) isnan(x)
#define m_isinf(x) isinf(x)
#define m_isfinite(x) isfinite(x)

#define m_mulsum(x,y,z) {z += x*y;}
#define m_mulsum_init INT2FIX(0)

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

static inline dtype pow_int(dtype x, int p)
{
    dtype r=1;
    switch(p) {
    case 0: return 1;
    case 1: return x;
    case 2: return x*x;
    case 3: return x*x*x;
    case 4: x=x*x; return x*x;
    }
    if (p<0)  return 1/pow_int(x,-p);
    if (p>64) return pow(x,p);
    while (p) {
        if (p&1) r *= x;
        x *= x;
        p >>= 1;
    }
    return r;
}


static inline dtype f_sum(size_t n, char *p, ssize_t stride)
{
    size_t i=n;
    dtype x,y=0;

    for (; i--;) {
        x = *(dtype*)p;
        if (!isnan(x)) {
            y += x;
        }
        p += stride;
    }
    return y;
}

static inline dtype f_mean(size_t n, char *p, ssize_t stride)
{
    size_t i=n;
    size_t count=0;
    dtype x,y=0;

    for (; i--;) {
        x = *(dtype*)p;
        if (!isnan(x)) {
            y += x;
            count++;
        }
        p += stride;
    }
    return y/count;
}

static inline dtype f_var(size_t n, char *p, ssize_t stride)
{
    size_t i=n;
    size_t count=0;
    dtype x,y=0;
    dtype a,m;

    m = f_mean(n,p,stride);

    for (; i--;) {
        x = *(dtype*)p;
        if (!isnan(x)) {
            a = x - m;
            y += a*a;
            count++;
        }
        p += stride;
    }
    return y/(count-1);
}

static inline dtype f_stddev(size_t n, char *p, ssize_t stride)
{
    return m_sqrt(f_var(n,p,stride));
}

static inline dtype f_rms(size_t n, char *p, ssize_t stride)
{
    size_t i=n;
    size_t count=0;
    dtype x,y=0;

    for (; i--;) {
        x = *(dtype*)p;
        if (!isnan(x)) {
            y += x*x;
            count++;
        }
        p += stride;
    }
    return m_sqrt(y/count);
}

static inline dtype f_min(size_t n, char *p, ssize_t stride)
{
    dtype x,y;
    size_t i=n;

    y = *(dtype*)p;
    p += stride;
    i--;
    for (; i--;) {
        x = *(dtype*)p;
        if (!isnan(x) && (isnan(y) || x<y)) {
            y = x;
        }
        p += stride;
    }
    return y;
}

static inline dtype f_max(size_t n, char *p, ssize_t stride)
{
    dtype x,y;
    size_t i=n;

    y = *(dtype*)p;
    p += stride;
    i--;
    for (; i--;) {
        x = *(dtype*)p;
        if (!isnan(x) && (isnan(y) || x>y)) {
            y = x;
        }
        p += stride;
    }
    return y;
}
