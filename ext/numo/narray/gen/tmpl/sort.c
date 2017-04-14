<% (is_float ? ["_ignan","_prnan"] : [""]).each do |j| %>
static void
<%=c_iter%><%=j%>(na_loop_t *const lp)
{
    size_t n;
    char *ptr;
    ssize_t step;

    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, ptr, step);
    <%=tp%>_qsort<%=j%>(ptr, n, step);
}
<% end %>

/*
  <%=method.capitalize%> of self.
  @overload <%=method%>(axis:nil, nan:false)
  @param [Numeric,Array,Range] axis  Affected dimensions.
  @param [TrueClass] nan  If true, propagete NaN. If false, ignore NaN.
  @return [Numo::<%=class_name%>] returns result of <%=method%>.
  @example
      Numo::DFloat[3,4,1,2].sort => Numo::DFloat[1,2,3,4]
*/
static VALUE
<%=c_func%>(int argc, VALUE *argv, VALUE self)
{
    int nan = 0;
    VALUE reduce;
    ndfunc_arg_in_t ain[2] = {{OVERWRITE,0},{sym_reduce,0}};
    ndfunc_t ndf = {0, STRIDE_LOOP|NDF_FLAT_REDUCE, 2,0, ain,0};

    if (!TEST_INPLACE(self)) {
        self = na_copy(self);
    }
    reduce = na_reduce_dimension(argc, argv, 1, &self, &nan); // v[0] = self
<% if is_float %>
    if (nan) {
        ndf.func = <%=c_iter%>_prnan;
    } else {
        ndf.func = <%=c_iter%>_ignan;
    }
<% else %>
    ndf.func = <%=c_iter%>;
<% end %>
    na_ndloop(&ndf, 2, self, reduce);
    return self;
}
