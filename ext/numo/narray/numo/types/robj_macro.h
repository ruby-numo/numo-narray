#define m_zero INT2FIX(0)
#define m_one  INT2FIX(1)

#define m_num_to_data(x) (x)
#define m_data_to_num(x) (x)

#define m_from_double(x) rb_float_new(x)
#define m_from_real(x)   rb_float_new(x)

#define m_add(x,y)     rb_funcall(x,'+',1,y)
#define m_sub(x,y)     rb_funcall(x,'-',1,y)
#define m_mul(x,y)     rb_funcall(x,'*',1,y)
#define m_div(x,y)     rb_funcall(x,'/',1,y)
#define m_mod(x,y)     rb_funcall(x,'%',1,y)
#define m_divmod(x,y,a,b)                               \
    {x = rb_funcall(x,id_divmod,1,y);                   \
     a = RARRAY_PTR(x)[0]; b = RARRAY_PTR(x)[0];}
#define m_pow(x,y)     rb_funcall(x,id_pow,1,y)
#define m_pow_int(x,y) rb_funcall(x,id_pow,1,y)

#define m_abs(x)       rb_funcall(x,id_abs,0)
#define m_minus(x)     rb_funcall(x,id_minus,0)
#define m_reciprocal(x)   rb_funcall(x,id_reciprocal,0)
#define m_square(x)    rb_funcall(x,'*',1,x)
#define m_floor(x)     rb_funcall(x,id_floor,0)
#define m_round(x)     rb_funcall(x,id_round,0)
#define m_ceil(x)      rb_funcall(x,id_ceil,0)
#define m_trunc(x)     rb_funcall(x,id_truncate,0)
#define m_sign(x)      rb_funcall(x,id_ufo,1,INT2FIX(0))

#define m_eq(x,y)      RTEST(rb_funcall(x,id_eq,1,y))
#define m_ne(x,y)      RTEST(rb_funcall(x,id_ne,1,y))
#define m_gt(x,y)      RTEST(rb_funcall(x,id_gt,1,y))
#define m_ge(x,y)      RTEST(rb_funcall(x,id_ge,1,y))
#define m_lt(x,y)      RTEST(rb_funcall(x,id_lt,1,y))
#define m_le(x,y)      RTEST(rb_funcall(x,id_le,1,y))

#define m_bit_and(x,y) rb_funcall(x,id_bit_and,1,y)
#define m_bit_or(x,y)  rb_funcall(x,id_bit_or, 1,y)
#define m_bit_xor(x,y) rb_funcall(x,id_bit_xor,1,y)
#define m_bit_not(x)   rb_funcall(x,id_bit_not,0)

#define m_left_shift(x,y) RTEST(rb_funcall(x,id_left_shift,1,y))
#define m_right_shift(x,y) RTEST(rb_funcall(x,id_right_shift,1,y))

#define m_isnan(x)     ((rb_respond_to(x,id_nan_p)) ? RTEST(rb_funcall(x,id_nan_p,0)) : 0)
#define m_isinf(x)     ((rb_respond_to(x,id_infinite_p)) ? RTEST(rb_funcall(x,id_infinite_p,0)) : 0)
#define m_isfinite(x)  ((rb_respond_to(x,id_finite_p)) ? RTEST(rb_funcall(x,id_finite_p,0)) : 0)

#define m_mulsum(x,y,z) {z = m_add(m_mul(x,y),z);}
#define m_mulsum_init INT2FIX(0)

#define m_sprintf(s,x) robj_sprintf(s,x)

static inline int robj_sprintf(char *s, VALUE x) {
    VALUE v = rb_funcall(x,rb_intern("to_s"),0);
    return sprintf(s,"%s",StringValuePtr(v));
}

static inline dtype f_sum(size_t n, char *p, ssize_t stride)
{
    size_t i=n;
    dtype x,y=INT2FIX(0);

    for (; i--;) {
        x = *(dtype*)p;
        if (!m_isnan(x)) {
            y = m_add(x,y);
        }
        p += stride;
    }
    return y;
}

static inline dtype f_prod(size_t n, char *p, ssize_t stride)
{
    size_t i=n;
    dtype x,y=INT2FIX(1);

    for (; i--;) {
        x = *(dtype*)p;
        if (!m_isnan(x)) {
            y = m_mul(x,y);
        }
        p += stride;
    }
    return y;
}

static inline dtype f_mean(size_t n, char *p, ssize_t stride)
{
    size_t i=n;
    size_t count=0;
    dtype x,y=INT2FIX(0);

    for (; i--;) {
        x = *(dtype*)p;
        if (!m_isnan(x)) {
            y = m_add(x,y);
            count++;
        }
        p += stride;
    }
    return m_div(y,DBL2NUM(count));
}

static inline dtype f_var(size_t n, char *p, ssize_t stride)
{
    size_t i=n;
    size_t count=0;
    dtype x,m;
    dtype y=INT2FIX(0);

    m = f_mean(n,p,stride);

    for (; i--;) {
        x = *(dtype*)p;
        if (!m_isnan(x)) {
            y = m_add(y,m_square(m_abs(m_sub(x,m))));
            count++;
        }
        p += stride;
    }
    return m_div(y,DBL2NUM(count-1));
}

static inline dtype f_stddev(size_t n, char *p, ssize_t stride)
{
    VALUE m = rb_const_get(rb_mKernel,rb_intern("Math"));
    return rb_funcall(m,rb_intern("sqrt"),1,f_var(n,p,stride));
}

static inline dtype f_rms(size_t n, char *p, ssize_t stride)
{
    size_t i=n;
    size_t count=0;
    dtype x;
    dtype y=INT2FIX(0);
    VALUE m;

    for (; i--;) {
        x = *(dtype*)p;
        if (!m_isnan(x)) {
            y = m_add(y,m_square(m_abs(x)));
            count++;
        }
        p += stride;
    }
    y = m_div(y,DBL2NUM(count));
    m = rb_const_get(rb_mKernel,rb_intern("Math"));
    return rb_funcall(m,rb_intern("sqrt"),1,y);
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
        if (!m_isnan(x) && (m_isnan(y) || m_lt(x,y))) {
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
        if (!m_isnan(x) && (m_isnan(y) || m_gt(x,y))) {
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
        if (!m_isnan(x) && (m_isnan(y) || m_lt(x,y))) {
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
        if (!m_isnan(x) && (m_isnan(y) || m_gt(x,y))) {
            y = x;
            j = i;
        }
    }
    return j;
}

static inline dtype f_seq(dtype x, dtype y, size_t c)
{
    y = m_mul(y,SIZET2NUM(c));
    return m_add(x,y);
}
