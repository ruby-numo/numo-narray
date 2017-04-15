static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t   i;
    char    *p1, *p2, *p3;
    ssize_t  s1, s2, s3;
    dtype    x;
    int      y;
    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR(lp, 2, p3, s3);
    for (; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        x = m_<%=name%>(x,&y);
        SET_DATA_STRIDE(p2,s2,dtype,x);
        SET_DATA_STRIDE(p3,s3,int32_t,y);
    }
}

/*
  split the number x into a normalized fraction and an exponent.
  Returns [mantissa, exponent], where x = mantissa * 2**exponent.

  @overload <%=name%>(x)
  @param [Numo::NArray,Numeric]  x
  @return [Numo::<%=class_name%>,Numo::Int32]  mantissa and exponent.

*/
static VALUE
<%=c_func(1)%>(VALUE mod, VALUE a1)
{
    ndfunc_arg_in_t ain[1] = {{cT,0}};
    ndfunc_arg_out_t aout[2] = {{cT,0},{numo_cInt32,0}};
    ndfunc_t ndf = { <%=c_iter%>, STRIDE_LOOP, 1,2, ain,aout };
    return na_ndloop(&ndf, 1, a1);
}
