static void
<%=c_iterator%>(na_loop_t *const lp)
{
    size_t  n;
    size_t  p1, p3;
    ssize_t s1, s3;
    size_t *idx1, *idx3;
    int     o1, l1, r1, len;
    BIT_DIGIT *a1, *a3;
    BIT_DIGIT  x;
    BIT_DIGIT  y;

    INIT_COUNTER(lp, n);
    INIT_PTR_BIT(lp, 0, a1, p1, s1, idx1);
    INIT_PTR_BIT(lp, 1, a3, p3, s3, idx3);
    if (s1!=1 || s3!=1 || idx1 || idx3) {
        for (; n--;) {
            LOAD_BIT_STEP(a1, p1, s1, idx1, x);
            y = m_<%=op%>(x);
            STORE_BIT_STEP(a3, p3, s3, idx3, y);
        }
    } else {
        o1 =  p1 % NB;
        o1 -= p3;
        l1 =  NB+o1;
        r1 =  NB-o1;
        if (p3>0 || n<NB) {
            len = NB - p3;
            if ((int)n<len) len=n;
            if (o1>=0) x = *a1>>o1;
            else       x = *a1<<-o1;
            if (p1+len>NB)  x |= *(a1+1)<<r1;
            a1++;
            y = m_<%=op%>(x);
            *a3 = y & (SLB(len)<<p3) | *a3 & ~(SLB(len)<<p3);
            a3++;
            n -= len;
        }
        if (o1==0) {
            for (; n>=NB; n-=NB) {
                x = *(a1++);
                y = m_<%=op%>(x);
                *(a3++) = y;
            }
        } else {
            for (; n>=NB; n-=NB) {
                x = *a1>>o1;
                if (o1<0)  x |= *(a1-1)>>l1;
                if (o1>0)  x |= *(a1+1)<<r1;
                a1++;
                y = m_<%=op%>(x);
                *(a3++) = y;
            }
        }
        if (n>0) {
            x = *a1>>o1;
            if (o1<0)  x |= *(a1-1)>>l1;
            y = m_<%=op%>(x);
            *a3 = y & SLB(n) | *a3 & BALL<<n;
        }
    }
}

static VALUE
<%=c_instance_method%>(VALUE a1)
{
     ndfunc_t *func;
     VALUE v;
     func = ndfunc_alloc(<%=c_iterator%>, FULL_LOOP,
                         1, 1, cBit, cBit);
     v = ndloop_do(func, 1, a1);
     ndfunc_free(func);
     return v;
}
