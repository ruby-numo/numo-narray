static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t    i;
    char     *p1;
    BIT_DIGIT *a2;
    size_t    p2;
    ssize_t   s1, s2;
    size_t   *idx1;
    dtype     x;
    BIT_DIGIT b;
    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR_BIT(lp, 1, a2, p2, s2);
    if (idx1) {
        for (; i--;) {
            GET_DATA_INDEX(p1,idx1,dtype,x);
            b = (m_<%=name%>(x)) ? 1:0;
            STORE_BIT(a2,p2,b);
            p2+=s2;
        }
    } else {
        for (; i--;) {
            GET_DATA_STRIDE(p1,s1,dtype,x);
            b = (m_<%=name%>(x)) ? 1:0;
            STORE_BIT(a2,p2,b);
            p2+=s2;
        }
    }
}

/*
  Condition of <%=name%>.
  @overload <%=name%>
  @return [Numo::Bit] Condition of <%=name%>.
*/
static VALUE
<%=c_func(0)%>(VALUE self)
{
    ndfunc_arg_in_t ain[1] = {{cT,0}};
    ndfunc_arg_out_t aout[1] = {{numo_cBit,0}};
    ndfunc_t ndf = { <%=c_iter%>, FULL_LOOP, 1, 1, ain, aout };

    return na_ndloop(&ndf, 1, self);
}
