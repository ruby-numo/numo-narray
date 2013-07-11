#define m_zero 0
#define m_one  1

#define m_from_double(x) (x)
#define m_from_real(x) (x)

#define m_add(x,y) ((x)+(y))
#define m_sub(x,y) ((x)-(y))
#define m_mul(x,y) ((x)*(y))
#define m_div(x,y) ((x)/(y))
#define m_mod(x,y) ((x)%(y))
#define m_pow(x,y) pow_int(x,y)
#define m_pow_int(x,y) pow_int(x,y)

#define m_bit_and(x,y) ((x)&(y))
#define m_bit_or(x,y)  ((x)|(y))
#define m_bit_xor(x,y) ((x)^(y))
#define m_bit_not(x)   (~(x))

#define m_abs(x)     (x)
#define m_minus(x)   (-(x))
#define m_inverse(x) int_inverse(x)
#define m_square(x)  ((x)*(x))

#define m_eq(x,y) ((x)==(y))
#define m_ne(x,y) ((x)!=(y))
#define m_gt(x,y) ((x)>(y))
#define m_ge(x,y) ((x)>=(y))
#define m_lt(x,y) ((x)<(y))
#define m_le(x,y) ((x)<=(y))

#define m_sum(x,y) {{y+=x;}}
#define m_sum_init INT2FIX(0)
#define m_min(x,y) {if (!isnan(x) && (isnan(y) || y>x)) {y=x;}}
#define m_min_init nary_init_accum_aref0(self, accum)
#define m_max(x,y) {if (!isnan(x) && (isnan(y) || y<x)) {y=x;}}
#define m_max_init nary_init_accum_aref0(self, accum)

#define cmp(a,b)                                        \
    ((qsort_cast(a)==qsort_cast(b)) ? 0 :               \
     (qsort_cast(a) > qsort_cast(b)) ? 1 : -1)
#define cmpgt(a,b)                              \
    (qsort_cast(a) > qsort_cast(b))


static inline dtype int_inverse(dtype x) {
    switch (x) {
    case 1:
        return 1;
    case 0:
        rb_raise(rb_eZeroDivError, "divided by 0");
    default:
        return 0;
    }
}

static dtype pow_int(dtype x, int p)
{
    dtype r = m_one;
    switch(p) {
    case 0: return 1;
    case 1: return x;
    case 2: return x*x;
    case 3: return x*x*x;
    }
    while (p) {
        if (p&1) r *= x;
        x *= x;
        p >>= 1;
    }
    return r;
}
