<% (is_float ? ["","_nan"] : [""]).each do |j|
   [64,32].each do |i| %>
#define idx_t int<%=i%>_t
static void
<%=c_iter%>_arg<%=i%><%=j%>(na_loop_t *const lp)
{
    size_t   n, idx;
    char    *d_ptr, *o_ptr;
    ssize_t  d_step;

    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, d_ptr, d_step);

    idx = f_<%=name[3..5]%>_index<%=j%>(n,d_ptr,d_step);

    o_ptr = NDL_PTR(lp,1);
    *(idx_t*)o_ptr = (idx_t)idx;
}
#undef idx_t
<% end;end %>

/*
  Index of the <%=name[3..5]%>imum value.
<% if is_float %>
  @overload <%=name%>(axis:nil, nan:false)
  @param [TrueClass] nan  If true, apply NaN-aware algorithm (return NaN posision if exist).
<% else %>
  @overload <%=name%>(axis:nil)
<% end %>
  @param [Numeric,Array,Range] axis  Finds <%=name[3..5]%>imum values along the axis and returns **indices along the axis**.
  @return [Integer,Numo::Int] returns the result indices.
  @see #<%=name[3..5]%>_index
  @see #<%=name[3..5]%>

  @example
<% case name; when /min/ %>
      a = Numo::NArray[3,4,1,2]
      a.argmin  #=> 2

      b = Numo::NArray[[3,4,1],[2,0,5]]
      b.argmin                       #=> 4
      b.argmin(axis:1)               #=> [2, 1]
      b.argmin(axis:0)               #=> [1, 1, 0]
      b.at(b.argmin(axis:0), 0..-1)  #=> [2, 0, 1]
<% when /max/ %>
      a = Numo::NArray[3,4,1,2]
      a.argmax  #=> 1

      b = Numo::NArray[[3,4,1],[2,0,5]]
      b.argmax                       #=> 5
      b.argmax(axis:1)               #=> [1, 2]
      b.argmax(axis:0)               #=> [0, 0, 1]
      b.at(b.argmax(axis:0), 0..-1)  #=> [3, 4, 5]
<% end %>
 */
static VALUE
<%=c_func(-1)%>(int argc, VALUE *argv, VALUE self)
{
    narray_t *na;
    VALUE reduce;
    ndfunc_arg_in_t ain[2] = {{Qnil,0},{sym_reduce,0}};
    ndfunc_arg_out_t aout[1] = {{0,0,0}};
    ndfunc_t ndf = {0, STRIDE_LOOP_NIP|NDF_FLAT_REDUCE|NDF_EXTRACT, 2,1, ain,aout};

    GetNArray(self,na);
    if (na->ndim==0) {
        return INT2FIX(0);
    }
    if (na->size > (~(u_int32_t)0)) {
        aout[0].type = numo_cInt64;
        ndf.func = <%=c_iter%>_arg64;
      <% if is_float %>
        reduce = na_reduce_dimension(argc, argv, 1, &self, &ndf, <%=c_iter%>_arg64_nan);
      <% else %>
        reduce = na_reduce_dimension(argc, argv, 1, &self, &ndf, 0);
      <% end %>
    } else {
        aout[0].type = numo_cInt32;
        ndf.func = <%=c_iter%>_arg32;
      <% if is_float %>
        reduce = na_reduce_dimension(argc, argv, 1, &self, &ndf, <%=c_iter%>_arg32_nan);
      <% else %>
        reduce = na_reduce_dimension(argc, argv, 1, &self, &ndf, 0);
      <% end %>
    }

    return na_ndloop(&ndf, 2, self, reduce);
}
