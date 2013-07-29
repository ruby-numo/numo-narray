static void
<%=c_iterator%>(na_loop_t *const lp)
{
    size_t  i;
    char    *p1, *p2, *p3;
    ssize_t s1, s2, s3;
    size_t  *idx1, *idx2, *idx3;
    dtype    x, y;
    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2, idx2);
    INIT_PTR(lp, 2, p3, s3, idx3);
    for (; i--;) {
        x = *(dtype*)p1;
        p1+=s1;
        y = *(dtype*)p2;
        p2+=s2;
        x = m_<%=op%>(x,y);
        *(dtype*)p3 = x;
        p3+=s3;
    }
}

/*
  Calculate <%=op%>(a1,a2).
  @overload <%=op%>(a1,a2)
  @param [NArray,Numeric] a1  first value
  @param [NArray,Numeric] a2  second value
  @return [NArray::<%=class_name%>] <%=op%>(a1,a2).
*/
static VALUE
<%=c_singleton_method%>(VALUE mod, VALUE a1, VALUE a2)
{
    ndfunc_arg_in_t ain[2] = {{cT,0},{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { <%=c_iterator%>, STRIDE_LOOP, 2, 1, ain, aout, 0 };
    return na_ndloop(&ndf, 2, a1, a2);
}
