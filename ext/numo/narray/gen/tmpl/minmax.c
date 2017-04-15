<% (is_float ? ["","_nan"] : [""]).each do |j| %>
static void
<%=c_iter%><%=j%>(na_loop_t *const lp)
{
    size_t   n;
    char    *p1;
    ssize_t  s1;
    dtype    xmin,xmax;

    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1);

    f_<%=name%><%=j%>(n,p1,s1,&xmin,&xmax);

    *(dtype*)(lp->args[1].ptr + lp->args[1].iter[0].pos) = xmin;
    *(dtype*)(lp->args[2].ptr + lp->args[2].iter[0].pos) = xmax;
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
  @return [Numo::<%=class_name%>,Numo::<%=class_name%>] min and max of self.
*/
static VALUE
<%=c_func(-1)%>(int argc, VALUE *argv, VALUE self)
{
    int ignore_nan = 0;
    VALUE reduce;
    ndfunc_arg_in_t ain[2] = {{cT,0},{sym_reduce,0}};
    ndfunc_arg_out_t aout[2] = {{cT,0},{cT,0}};
    ndfunc_t ndf = {<%=c_iter%>, STRIDE_LOOP_NIP|NDF_FLAT_REDUCE|NDF_EXTRACT, 2,2, ain,aout};

    reduce = na_reduce_dimension(argc, argv, 1, &self, &ignore_nan);
<% if is_float %>
    if (ignore_nan) {
        ndf.func = <%=c_iter%>_nan;
    }
<% end %>
    return na_ndloop(&ndf, 2, self, reduce);
}
