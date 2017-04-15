<% (is_float ? ["","_nan"] : [""]).each do |j| %>
static void
<%=c_iter%><%=j%>(na_loop_t *const lp)
{
    size_t   n;
    char    *p1, *p2;
    ssize_t  s1;

    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1);
    p2 = lp->args[1].ptr + lp->args[1].iter[0].pos;

    *(<%=dtype%>*)p2 = f_<%=name%><%=j%>(n,p1,s1);
}
<% end %>

/*
  <%=name%> of self.
<% if is_float %>
  @overload <%=name%>(axis:nil, nan:false)
  @param [TrueClass] nan  If true, propagete NaN. If false, ignore NaN.
<% else %>
  @overload <%=name%>(axis:nil)
<% end %>
  @param [Numeric,Array,Range] axis  Affected dimensions.
  @return [Numo::<%=class_name%>] returns result of <%=name%>.
*/
static VALUE
<%=c_func(-1)%>(int argc, VALUE *argv, VALUE self)
{
    int ignore_nan = 0;
    VALUE v, reduce;
    ndfunc_arg_in_t ain[2] = {{cT,0},{sym_reduce,0}};
    ndfunc_arg_out_t aout[1] = {{<%=tpclass%>,0}};
    ndfunc_t ndf = { <%=c_iter%>, STRIDE_LOOP_NIP|NDF_FLAT_REDUCE, 2, 1, ain, aout };

    reduce = na_reduce_dimension(argc, argv, 1, &self, &ignore_nan);
<% if is_float %>
    if (ignore_nan) {
        ndf.func = <%=c_iter%>_nan;
    }
<% end %>
    v =  na_ndloop(&ndf, 2, self, reduce);
<% if tpclass == "cT" %>
    return <%=type_name%>_extract(v);
<% else %>
    return rb_funcall(v,rb_intern("extract"),0);
<% end %>
}
