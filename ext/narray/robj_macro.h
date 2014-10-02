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
#define m_inverse(x)   rb_funcall(x,id_inverse,0)
#define m_square(x)    rb_funcall(x,id_square,0)
#define m_floor(x)     rb_funcall(x,id_floor,0)
#define m_round(x)     rb_funcall(x,id_round,0)
#define m_ceil(x)      rb_funcall(x,id_ceil,0)

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

#define m_isnan(x)     RTEST(rb_funcall(x,id_isnan,0))
#define m_isinf(x)     RTEST(rb_funcall(x,id_isinf,0))
#define m_isfinite(x)  RTEST(rb_funcall(x,id_isfinite,0))

#define m_sum(x,y) {if (!m_isnan(x)) {y = m_add(x,y);}}
#define m_sum_init INT2FIX(0)
#define m_min(x,y) {if (!(m_isnan(x) && m_isnan(x)) || m_gt(x,y)) {y = x;}}
#define m_max(x,y) {if (!(m_isnan(x) && m_isnan(x)) || m_lt(x,y)) {y = x;}}

#define m_rand to_res53(gen_rand64())
#define m_rand_norm(a) rand_norm(a)

#define m_sprintf(s,x) robj_sprintf(s,x)

static inline int robj_sprintf(char *s, VALUE x) {
    VALUE v = rb_funcall(x,rb_intern("to_s"),0);
    return sprintf(s,"%s",StringValuePtr(v));
}
