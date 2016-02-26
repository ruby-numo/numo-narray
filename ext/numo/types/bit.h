typedef int dtype;

#define m_load_data(ptr,pos) load_data(ptr,pos)

#define m_sprintf(s,x)   sprintf(s,"%1d",(int)(x))

#define m_bit_not(x)   (~(x))

#define m_bit_and(x,y) ((x)&(y))
#define m_bit_or(x,y)  ((x)|(y))
#define m_bit_xor(x,y) ((x)^(y))
#define m_eq(x,y)      (~((x)^(y)))

#define m_count_true(x)  (x!=0)
#define m_count_false(x) (x==0)

static inline dtype load_data(void *ptr, size_t pos) {
    return (((BIT_DIGIT*)(ptr))[(pos)/NB]>>((pos)%NB)) & 1u;
}
