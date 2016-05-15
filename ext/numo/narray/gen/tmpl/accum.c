static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t   i;
    char    *p1, *p2;
    ssize_t  s1;
    dtype    x, y;

    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1);
    p2 = lp->args[1].ptr + lp->args[1].iter[0].pos;
    //INIT_PTR(lp, 1, p2, s2);
    //printf("n=%ld,s1=%ld,s2=%ld ",i,s1,s2);
    // Reduce loop
    GET_DATA_STRIDE(p1,s1,dtype,y);
    i--;
    for (; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        m_<%=method%>(x,y);
    }
    SET_DATA(p2,dtype,y);
}

/*
  <%=method.capitalize%> of self.
  @overload <%=method%>(*args)
  @param [Array of Numeric,Range] args  Affected dimensions.
  @return [Numo::<%=class_name%>] <%=method%> of self.
*/
static VALUE
<%=c_func%>(int argc, VALUE *argv, VALUE self)
{
    VALUE v, reduce;
    ndfunc_arg_in_t ain[2] = {{cT,0},{sym_reduce,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { <%=c_iter%>, STRIDE_LOOP_NIP|NDF_FLAT_REDUCE, 2, 1, ain, aout };

    reduce = na_reduce_dimension(argc, argv, 1, &self);
    v =  na_ndloop(&ndf, 2, self, reduce);
    return numo_<%=tp%>_extract(v);
}
