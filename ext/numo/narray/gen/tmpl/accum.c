static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t   n;
    char    *p1, *p2;
    ssize_t  s1;

    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1);
    p2 = lp->args[1].ptr + lp->args[1].iter[0].pos;

    *(<%=dtype%>*)p2 = f_<%=method%>(n,p1,s1);
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
    ndfunc_arg_out_t aout[1] = {{<%=tpclass%>,0}};
    ndfunc_t ndf = { <%=c_iter%>, STRIDE_LOOP_NIP|NDF_FLAT_REDUCE, 2, 1, ain, aout };

    reduce = na_reduce_dimension(argc, argv, 1, &self);
    v =  na_ndloop(&ndf, 2, self, reduce);
<% if tpclass == "cT" %>
    return numo_<%=tp%>_extract(v);
<% else %>
    return rb_funcall(v,rb_intern("extract"),0);
<% end %>
}
