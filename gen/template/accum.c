static void
<%=c_iterator%>(na_loop_t *const lp)
{
    size_t   i;
    char    *p1, *p2;
    ssize_t  s1, s2;
    size_t  *idx1;
    dtype    x, y;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2);
    if (s2==0) {
        // Reduce loop
        GET_DATA(p2,dtype,y);
        if (idx1) {
            for (; i--;) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                m_<%=op%>(x,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                m_<%=op%>(x,y);
            }
        }
        SET_DATA(p2,dtype,y);
    } else {
        if (idx1) {
            for (; i--;) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                GET_DATA(p2,dtype,y);
                m_<%=op%>(x,y);
                SET_DATA_STRIDE(p2,s2,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                GET_DATA(p2,dtype,y);
                m_<%=op%>(x,y);
                SET_DATA_STRIDE(p2,s2,dtype,y);
            }
        }
    }
}

/*
  <%=op.capitalize%> of self.
  @overload <%=op%>(*args)
  @param [Array of Numeric,Range] args  Affected dimensions.
  @return [NArray::<%=class_name%>] <%=op%> of self.
*/
static VALUE
<%=c_instance_method%>(int argc, VALUE *argv, VALUE self)
{
    VALUE v, reduce;
    ndfunc_arg_in_t ain[3] = {{cT,0},{sym_reduce,0},{sym_init,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { <%=c_iterator%>, FULL_LOOP_NIP, 3, 1, ain, aout };

    reduce = na_reduce_dimension(argc, argv, self);
    v =  na_ndloop(&ndf, 3, self, reduce, m_<%=op%>_init);
    return nary_<%=tp%>_extract(v);
}
