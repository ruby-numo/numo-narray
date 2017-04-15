<% (is_float ? ["","_nan"] : [""]).each do |j| %>
static void
<%=c_iter%><%=j%>(na_loop_t *const lp)
{
    size_t   i;
    char    *p1, *p2;
    ssize_t  s1, s2;
    dtype    x, y;

    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    //printf("i=%lu p1=%lx s1=%lu p2=%lx s2=%lu\n",i,(size_t)p1,s1,(size_t)p2,s2);

    GET_DATA_STRIDE(p1,s1,dtype,x);
    SET_DATA_STRIDE(p2,s2,dtype,x);
    //printf("i=%lu x=%f\n",i,x);
    for (i--; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,y);
        m_<%=name%><%=j%>(x,y);
        SET_DATA_STRIDE(p2,s2,dtype,x);
        //printf("i=%lu x=%f\n",i,x);
    }
}
<% end %>

/*
  <%=name%> of self.
  @overload <%=name%>(axis:nil, nan:false)
  @param [Numeric,Array,Range] axis  Affected dimensions.
  @param [TrueClass] nan  If true, propagete NaN. If false, ignore NaN.
  @return [Numo::<%=class_name%>] <%=name%> of self.
*/
static VALUE
<%=c_func(-1)%>(int argc, VALUE *argv, VALUE self)
{
    int ignore_nan = 0;
    VALUE reduce;
    ndfunc_arg_in_t ain[2] = {{cT,0},{sym_reduce,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { <%=c_iter%>, STRIDE_LOOP|NDF_FLAT_REDUCE|NDF_CUM,
                     2, 1, ain, aout };

    reduce = na_reduce_dimension(argc, argv, 1, &self, &ignore_nan);
<% if is_float %>
    if (ignore_nan) {
        ndf.func = <%=c_iter%>_nan;
    }
<% end %>
    return na_ndloop(&ndf, 2, self, reduce);
}
