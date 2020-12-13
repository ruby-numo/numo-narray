static void
<%=c_iter%>(na_loop_t *const lp)
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
    INIT_PTR_BIT_IDX(lp, 0, a1, p1, s1, idx1);
    INIT_PTR_BIT_IDX(lp, 1, a3, p3, s3, idx3);
    if (s1!=1 || s3!=1 || idx1 || idx3) {
        for (; n--;) {
            LOAD_BIT_STEP(a1, p1, s1, idx1, x);
            y = m_<%=name%>(x);
            STORE_BIT_STEP(a3, p3, s3, idx3, y);
        }
    } else {
        a1 += p1/NB;
        p1 %= NB;
        a3 += p3/NB;
        p3 %= NB;
        o1 =  p1-p3;
        l1 =  NB+o1;
        r1 =  NB-o1;
        if (p3>0 || n<NB) {
            len = NB - p3;
            if ((int)n<len) len=n;
            if (o1>=0) x = *a1>>o1;
            else       x = *a1<<-o1;
            if (p1+len>NB)  x |= *(a1+1)<<r1;
            a1++;
            y = m_<%=name%>(x);
            *a3 = (y & (SLB(len)<<p3)) | (*a3 & ~(SLB(len)<<p3));
            a3++;
            n -= len;
        }
        if (o1==0) {
            for (; n>=NB; n-=NB) {
                x = *(a1++);
                y = m_<%=name%>(x);
                *(a3++) = y;
            }
        } else {
            for (; n>=NB; n-=NB) {
                if (o1==0) {
                    x = *a1;
                } else if (o1>0) {
                    x = *a1>>o1  | *(a1+1)<<r1;
                } else {
                    x = *a1<<-o1 | *(a1-1)>>l1;
                }
                a1++;
                y = m_<%=name%>(x);
                *(a3++) = y;
            }
        }
        if (n>0) {
            if (o1==0) {
                x = *a1;
            } else if (o1>0) {
                x = *a1>>o1;
                if ((int)n>r1) {
                    x |= *(a1+1)<<r1;
                }
            } else {
                x = *(a1-1)>>l1;
                if ((int)n>-o1) {
                    x |= *a1<<-o1;
                }
            }
            y = m_<%=name%>(x);
            *a3 = (y & SLB(n)) | (*a3 & BALL<<n);
        }
    }
}

/*
  Unary <%=name%>.
  @overload <%=name%>
  @return [Numo::<%=class_name%>] <%=name%> of self.
*/
static VALUE
<%=c_func(0)%>(VALUE self)
{
    ndfunc_arg_in_t ain[1] = {{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = {<%=c_iter%>, FULL_LOOP, 1,1, ain,aout};

    return na_ndloop(&ndf, 1, self);
}
