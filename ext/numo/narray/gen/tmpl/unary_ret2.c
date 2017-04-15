static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t  i;
    char   *p1, *p2, *p3;
    ssize_t s1, s2, s3;
    dtype   x, y, z;
    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR(lp, 2, p3, s3);
    for (; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        m_<%=name%>(x,y,z);
        SET_DATA_STRIDE(p2,s2,dtype,y);
        SET_DATA_STRIDE(p3,s3,dtype,z);
    }
}

/*
  <%=name%> of self.
  @overload <%=name%>
  @return [Numo::<%=real_class_name%>] <%=name%> of self.
*/
static VALUE
<%=c_func(0)%>(VALUE self)
{
    ndfunc_arg_in_t ain[1] = {{cT,0}};
    ndfunc_arg_out_t aout[2] = {{cT,0},{cT,0}};
    ndfunc_t ndf = {<%=c_iter%>, STRIDE_LOOP, 1,2, ain,aout};

    return na_ndloop(&ndf, 1, self);
}
