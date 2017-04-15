static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t  n;
    size_t  p3;
    ssize_t s3;
    size_t *idx3;
    int     len;
    BIT_DIGIT *a3;
    BIT_DIGIT  y;
    VALUE x = lp->option;

    if (x==INT2FIX(0) || x==Qfalse) {
        y = 0;
    } else
    if (x==INT2FIX(1) || x==Qtrue) {
        y = ~(BIT_DIGIT)0;
    } else {
        rb_raise(rb_eArgError, "invalid value for Bit");
    }

    INIT_COUNTER(lp, n);
    INIT_PTR_BIT_IDX(lp, 0, a3, p3, s3, idx3);
    if (idx3) {
        y = y&1;
        for (; n--;) {
            STORE_BIT(a3, p3+*idx3, y); idx3++;
        }
    } else if (s3!=1) {
        y = y&1;
        for (; n--;) {
            STORE_BIT(a3, p3, y); p3+=s3;
        }
    } else {
        if (p3>0 || n<NB) {
            len = NB - p3;
            if ((int)n<len) len=n;
            *a3 = (y & (SLB(len)<<p3)) | (*a3 & ~(SLB(len)<<p3));
            a3++;
            n -= len;
        }
        for (; n>=NB; n-=NB) {
            *(a3++) = y;
        }
        if (n>0) {
            *a3 = (y & SLB(n)) | (*a3 & BALL<<n);
        }
    }
}

/*
  Fill elements with other.
  @overload <%=name%> other
  @param [Numeric] other
  @return [Numo::<%=class_name%>] self.
*/
static VALUE
<%=c_func(1)%>(VALUE self, VALUE val)
{
    ndfunc_arg_in_t ain[2] = {{OVERWRITE,0},{sym_option}};
    ndfunc_t ndf = {<%=c_iter%>, FULL_LOOP, 2,0, ain,0};

    na_ndloop(&ndf, 2, self, val);
    return self;
}
