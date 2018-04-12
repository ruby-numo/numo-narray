typedef BIT_DIGIT dtype;
typedef BIT_DIGIT rtype;
#define cT  numo_cBit
#define cRT cT

#define m_zero 0
#define m_one  1

#define m_abs(x)     (x)
#define m_sign(x)    (((x)==0) ? 0:1)

#define m_from_double(x) (((x)==0) ? 0 : 1)
#define m_from_real(x) (((x)==0) ? 0 : 1)
#define m_from_sint(x) (((x)==0) ? 0 : 1)
#define m_from_int32(x) (((x)==0) ? 0 : 1)
#define m_from_int64(x) (((x)==0) ? 0 : 1)
#define m_from_uint32(x) (((x)==0) ? 0 : 1)
#define m_from_uint64(x) (((x)==0) ? 0 : 1)
#define m_data_to_num(x) INT2FIX(x)
#define m_sprintf(s,x)   sprintf(s,"%1d",(int)(x))

#define m_copy(x)  (x)
#define m_not(x)   (~(x))
#define m_and(x,y) ((x)&(y))
#define m_or(x,y)  ((x)|(y))
#define m_xor(x,y) ((x)^(y))
#define m_eq(x,y)  (~((x)^(y)))
#define m_count_true(x)  ((x)!=0)
#define m_count_false(x) ((x)==0)

static inline BIT_DIGIT m_num_to_data(VALUE num) {
    if (RTEST(num)) {
        if (!RTEST(rb_equal(num,INT2FIX(0)))) {
            return 1;
        }
    }
    return 0;
}
