static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t  i;
    char   *p1, *p2;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    dtype   x;
    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);
    if (idx1) {
        if (idx2) {
            for (; i--;) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                x = m_<%=method%>(x);
                SET_DATA_INDEX(p2,idx2,dtype,x);
            }
        } else {
            for (; i--;) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                x = m_<%=method%>(x);
                SET_DATA_STRIDE(p2,s2,dtype,x);
            }
        }
    } else {
        if (idx2) {
            for (; i--;) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                x = m_<%=method%>(x);
                SET_DATA_INDEX(p2,idx2,dtype,x);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                x = m_<%=method%>(x);
                SET_DATA_STRIDE(p2,s2,dtype,x);
            }
        }
    }
}

/*
  Unary <%=method%>.
  @overload <%=op_map%>
  @return [NArray::<%=class_name%>] <%=method%> of self.
*/
static VALUE
<%=c_func%>(VALUE self)
{
    ndfunc_arg_in_t ain[1] = {{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { <%=c_iter%>, FULL_LOOP, 1, 1, ain, aout };

    return na_ndloop(&ndf, 1, self);
}
