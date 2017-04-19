static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t  i;
    char   *p1, *p2;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    dtype   x;
    <%=dtype%> y;
    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);
    if (idx1) {
        if (idx2) {
            for (; i--;) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                y = m_<%=name%>(x);
                SET_DATA_INDEX(p2,idx2,<%=dtype%>,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                y = m_<%=name%>(x);
                SET_DATA_STRIDE(p2,s2,<%=dtype%>,y);
            }
        }
    } else {
        if (idx2) {
            for (; i--;) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                y = m_<%=name%>(x);
                SET_DATA_INDEX(p2,idx2,<%=dtype%>,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                y = m_<%=name%>(x);
                SET_DATA_STRIDE(p2,s2,<%=dtype%>,y);
            }
        }
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
    ndfunc_arg_out_t aout[1] = {{<%=result_class%>,0}};
    ndfunc_t ndf = { <%=c_iter%>, FULL_LOOP, 1, 1, ain, aout };

    return na_ndloop(&ndf, 1, self);
}
