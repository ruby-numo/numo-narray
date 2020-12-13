static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t  n;
    size_t  p1, p2, p3;
    ssize_t s1, s2, s3;
    size_t *idx1, *idx2, *idx3;
    int     o1, o2, l1, l2, r1, r2, len;
    BIT_DIGIT *a1, *a2, *a3;
    BIT_DIGIT  x, y;

    INIT_COUNTER(lp, n);
    INIT_PTR_BIT_IDX(lp, 0, a1, p1, s1, idx1);
    INIT_PTR_BIT_IDX(lp, 1, a2, p2, s2, idx2);
    INIT_PTR_BIT_IDX(lp, 2, a3, p3, s3, idx3);
    if (s1!=1 || s2!=1 || s3!=1 || idx1 || idx2 || idx3) {
        for (; n--;) {
            LOAD_BIT_STEP(a1, p1, s1, idx1, x);
            LOAD_BIT_STEP(a2, p2, s2, idx2, y);
            x = m_<%=name%>(x,y);
            STORE_BIT_STEP(a3, p3, s3, idx3, x);
        }
    } else {
        a1 += p1/NB;
        p1 %= NB;
        a2 += p2/NB;
        p2 %= NB;
        a3 += p3/NB;
        p3 %= NB;
        o1 =  p1-p3;
        o2 =  p2-p3;
        l1 =  NB+o1;
        r1 =  NB-o1;
        l2 =  NB+o2;
        r2 =  NB-o2;
        if (p3>0 || n<NB) {
            len = NB - p3;
            if ((int)n<len) len=n;
            if (o1>=0) x = *a1>>o1;
            else       x = *a1<<-o1;
            if (p1+len>NB)  x |= *(a1+1)<<r1;
            a1++;
            if (o2>=0) y = *a2>>o2;
            else       y = *a2<<-o2;
            if (p2+len>NB)  y |= *(a2+1)<<r2;
            a2++;
            x = m_<%=name%>(x,y);
            *a3 = (x & (SLB(len)<<p3)) | (*a3 & ~(SLB(len)<<p3));
            a3++;
            n -= len;
        }
        if (o1==0 && o2==0) {
            for (; n>=NB; n-=NB) {
                x = *(a1++);
                y = *(a2++);
                x = m_<%=name%>(x,y);
                *(a3++) = x;
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
                if (o2==0) {
                    y = *a2;
                } else if (o2>0) {
                    y = *a2>>o2  | *(a2+1)<<r2;
                } else {
                    y = *a2<<-o2 | *(a2-1)>>l2;
                }
                a2++;
                x = m_<%=name%>(x,y);
                *(a3++) = x;
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
            if (o2==0) {
                y = *a2;
            } else if (o2>0) {
                y = *a2>>o2;
                if ((int)n>r2) {
                    y |= *(a2+1)<<r2;
                }
            } else {
                y = *(a2-1)>>l2;
                if ((int)n>-o2) {
                    y |= *a2<<-o2;
                }
            }
            x = m_<%=name%>(x,y);
            *a3 = (x & SLB(n)) | (*a3 & BALL<<n);
        }
    }
}

/*
  Binary <%=name%>.
  @overload <%=op_map%> other
  @param [Numo::NArray,Numeric] other
  @return [Numo::NArray] <%=name%> of self and other.
*/
static VALUE
<%=c_func(1)%>(VALUE self, VALUE other)
{
    ndfunc_arg_in_t ain[2] = {{cT,0},{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { <%=c_iter%>, FULL_LOOP, 2, 1, ain, aout };

    return na_ndloop(&ndf, 2, self, other);
}
