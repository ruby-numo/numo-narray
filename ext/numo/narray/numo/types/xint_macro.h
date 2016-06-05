#define m_zero 0
#define m_one  1

#define m_from_double(x) (x)
#define m_from_real(x) (x)

#define m_add(x,y) ((x)+(y))
#define m_sub(x,y) ((x)-(y))
#define m_mul(x,y) ((x)*(y))
#define m_div(x,y) ((x)/(y))
#define m_mod(x,y) ((x)%(y))
#define m_divmod(x,y,a,b) {a=(x)/(y); b=m_mod(x,y);}
#define m_pow(x,y) pow_int(x,y)
#define m_pow_int(x,y) pow_int(x,y)

#define m_bit_and(x,y) ((x)&(y))
#define m_bit_or(x,y)  ((x)|(y))
#define m_bit_xor(x,y) ((x)^(y))
#define m_bit_not(x)   (~(x))

#define m_minus(x)   (-(x))
#define m_inverse(x) int_inverse(x)
#define m_square(x)  ((x)*(x))

#define m_eq(x,y) ((x)==(y))
#define m_ne(x,y) ((x)!=(y))
#define m_gt(x,y) ((x)>(y))
#define m_ge(x,y) ((x)>=(y))
#define m_lt(x,y) ((x)<(y))
#define m_le(x,y) ((x)<=(y))
#define m_isnan(x) 0

#define m_mulsum(x,y,z) {z += x*y;}
#define m_mulsum_init INT2FIX(0)

#define cmp(a,b)                                        \
    ((qsort_cast(a)==qsort_cast(b)) ? 0 :               \
     (qsort_cast(a) > qsort_cast(b)) ? 1 : -1)
#define cmpgt(a,b)                              \
    (qsort_cast(a) > qsort_cast(b))


static inline dtype f_sum(size_t n, char *p, ssize_t stride)
{
    dtype x,y=0;
    size_t i=n;
    for (; i--;) {
        x = *(dtype*)p;
        y += x;
        p += stride;
    }
    return y;
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
        if (x < y) {
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
        if (x > y) {
            y = x;
        }
        p += stride;
    }
    return y;
}

static inline size_t f_min_index(size_t n, char *p, ssize_t stride)
{
    dtype x, y;
    size_t i, j=0;

    y = *(dtype*)p;
    for (i=1; i<n; i++) {
        x = *(dtype*)(p+i*stride);
        if (x < y) {
            y = x;
            j = i;
        }
    }
    return j;
}

static inline size_t f_max_index(size_t n, char *p, ssize_t stride)
{
    dtype x, y;
    size_t i, j=0;

    y = *(dtype*)p;
    for (i=1; i<n; i++) {
        x = *(dtype*)(p+i*stride);
        if (x > y) {
            y = x;
            j = i;
        }
    }
    return j;
}
