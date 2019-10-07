<% (is_float ? ["_ignan","_prnan"] : [""]).each do |j| %>
static void
<%=c_iter%><%=j%>(na_loop_t *const lp)
{
    size_t n;
    char *ptr;
    ssize_t step;

    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, ptr, step);
    <%=type_name%>_qsort<%=j%>(ptr, n, step);
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
  @param [Numeric,Array,Range] axis  Performs <%=name%> along the axis.
  @return [Numo::<%=class_name%>] returns result of <%=name%>.
  @example
      Numo::DFloat[3,4,1,2].sort #=> Numo::DFloat[1,2,3,4]
*/
static VALUE
<%=c_func(-1)%>(int argc, VALUE *argv, VALUE self)
{
    VALUE reduce;
    ndfunc_arg_in_t ain[2] = {{OVERWRITE,0},{sym_reduce,0}};
    ndfunc_t ndf = {0, NDF_HAS_LOOP|NDF_FLAT_REDUCE, 2,0, ain,0};

    if (!TEST_INPLACE(self)) {
        self = na_copy(self);
    }
  <% if is_float %>
    ndf.func = <%=c_iter%>_ignan;
    reduce = na_reduce_dimension(argc, argv, 1, &self, &ndf, <%=c_iter%>_prnan);
  <% else %>
    ndf.func = <%=c_iter%>;
    reduce = na_reduce_dimension(argc, argv, 1, &self, &ndf, 0);
  <% end %>
    na_ndloop(&ndf, 2, self, reduce);
    return self;
}
