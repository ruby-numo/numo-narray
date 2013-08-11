static void
<%=c_iterator%>(na_loop_t *const lp)
{
    size_t  i;
    char    *p1, *p2, *p3;
    ssize_t s1, s2, s3;
    dtype    x, y;
    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR(lp, 2, p3, s3);
    for (; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,dtype,y);
        x = m_<%=op%>(x,y);
        SET_DATA_STRIDE(p3,s3,dtype,x);
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
    ndfunc_t ndf = { <%=c_iterator%>, STRIDE_LOOP, 2, 1, ain, aout };
    return na_ndloop(&ndf, 2, a1, a2);
}
