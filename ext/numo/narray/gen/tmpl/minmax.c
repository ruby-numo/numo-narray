<% (is_float ? ["","_nan"] : [""]).each do |j| %>
static void
<%=c_iter%><%=j%>(na_loop_t *const lp)
{
    size_t   i;
    char    *p1;
    ssize_t  s1;
    dtype    x,xmin,xmax;

    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1);

    xmin = xmax = *(dtype*)p1;
    p1 += s1;
    i--;
    for (; i--;) {
        x = *(dtype*)p1;
<% if j=="" %>
        if (m_gt(x,xmax)) {
            xmax = x;
        }
        if (m_lt(x,xmin)) {
            xmin = x;
        }
<% else %>
        if (!m_isnan(x)) {
            if (m_isnan(xmax) || m_gt(x,xmax)) {
                xmax = x;
            }
            if (m_isnan(xmin) || m_lt(x,xmin)) {
                xmin = x;
            }
        }
<% end %>
        p1 += s1;
    }
    *(dtype*)(lp->args[1].ptr + lp->args[1].iter[0].pos) = xmin;
    *(dtype*)(lp->args[2].ptr + lp->args[2].iter[0].pos) = xmax;
}
<% end %>

/*
  <%=method.capitalize%> of self.
  @overload <%=method%>(*args)
  @param [Array of Numeric,Range] args  Affected dimensions.
  @return [Numo::<%=class_name%>,Numo::<%=class_name%>] min and max of self.
*/
static VALUE
<%=c_func%>(int argc, VALUE *argv, VALUE self)
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
