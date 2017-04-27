<% (is_float ? ["","_nan"] : [""]).each do |j|
   [64,32].each do |i| %>
#define idx_t int<%=i%>_t
static void
<%=c_iter%>_index<%=i%><%=j%>(na_loop_t *const lp)
{
    size_t   n, idx;
    char    *d_ptr, *i_ptr, *o_ptr;
    ssize_t  d_step, i_step;

    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, d_ptr, d_step);

    idx = f_<%=name%><%=j%>(n,d_ptr,d_step);

    INIT_PTR(lp, 1, i_ptr, i_step);
    o_ptr = NDL_PTR(lp,2);
    *(idx_t*)o_ptr = *(idx_t*)(i_ptr + i_step * idx);
}
#undef idx_t
<% end;end %>

/*
  <%=name%>. Return an index of result.
<% if is_float %>
  @overload <%=name%>(axis:nil, nan:false)
  @param [TrueClass] nan  If true, apply NaN-aware algorithm (return NaN posision if exist).
<% else %>
  @overload <%=name%>(axis:nil)
<% end %>
  @param [Numeric,Array,Range] axis  Affected dimensions.
  @return [Integer,Numo::Int] returns result index of <%=name%>.
  @example
      Numo::NArray[3,4,1,2].min_index => 3
 */
static VALUE
<%=c_func(-1)%>(int argc, VALUE *argv, VALUE self)
{
    narray_t *na;
    VALUE idx, reduce;
    ndfunc_arg_in_t ain[3] = {{Qnil,0},{Qnil,0},{sym_reduce,0}};
    ndfunc_arg_out_t aout[1] = {{0,0,0}};
    ndfunc_t ndf = {0, STRIDE_LOOP_NIP|NDF_FLAT_REDUCE|NDF_EXTRACT, 3,1, ain,aout};

    GetNArray(self,na);
    if (na->ndim==0) {
        return INT2FIX(0);
    }
    if (na->size > (~(u_int32_t)0)) {
        aout[0].type = numo_cInt64;
        idx = nary_new(numo_cInt64, na->ndim, na->shape);
        ndf.func = <%=c_iter%>_index64;
      <% if is_float %>
        reduce = na_reduce_dimension(argc, argv, 1, &self, &ndf, <%=c_iter%>_index64_nan);
      <% else %>
        reduce = na_reduce_dimension(argc, argv, 1, &self, &ndf, 0);
      <% end %>
    } else {
        aout[0].type = numo_cInt32;
        idx = nary_new(numo_cInt32, na->ndim, na->shape);
        ndf.func = <%=c_iter%>_index32;
      <% if is_float %>
        reduce = na_reduce_dimension(argc, argv, 1, &self, &ndf, <%=c_iter%>_index32_nan);
      <% else %>
        reduce = na_reduce_dimension(argc, argv, 1, &self, &ndf, 0);
      <% end %>
    }
    rb_funcall(idx, rb_intern("seq"), 0);

    return na_ndloop(&ndf, 3, self, idx, reduce);
}
