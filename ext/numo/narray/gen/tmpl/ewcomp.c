/*
  Element-wise <%=name%> of two arrays.

<% if is_float %>
  @overload <%=name%>(a1, a2, nan:false)
  @param [Numo::NArray,Numeric] a1  The array to be compared.
  @param [Numo::NArray,Numeric] a2  The array to be compared.
  @param [TrueClass] nan  If true, apply NaN-aware algorithm (return NaN if exist).
<% else %>
  @overload <%=name%>(a1, a2)
  @param [Numo::NArray,Numeric] a1,a2  The arrays holding the elements to be compared.
<% end %>
  @return [Numo::<%=class_name%>]
*/

<% (is_float ? ["","_nan"] : [""]).each do |j| %>
static void
<%=c_iter%><%=j%>(na_loop_t *const lp)
{
    size_t   i, n;
    char    *p1, *p2, *p3;
    ssize_t  s1, s2, s3;

    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR(lp, 2, p3, s3);

    for (i=0; i<n; i++) {
        dtype x, y, z;
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,dtype,y);
        GET_DATA(p3,dtype,z);
        z = f_<%=name%><%=j%>(x,y);
        SET_DATA_STRIDE(p3,s3,dtype,z);
    }
}
<% end %>

static VALUE
<%=c_func(-1)%>(int argc, VALUE *argv, VALUE mod)
{
    VALUE a1 = Qnil;
    VALUE a2 = Qnil;
    ndfunc_arg_in_t ain[2] = {{cT,0},{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { <%=c_iter%>, STRIDE_LOOP_NIP, 2, 1, ain, aout };

    <% if is_float %>
    VALUE kw_hash = Qnil;
    ID kw_table[1] = {id_nan};
    VALUE opts[1] = {Qundef};

    rb_scan_args(argc, argv, "20:", &a1, &a2, &kw_hash);
    rb_get_kwargs(kw_hash, kw_table, 0, 1, opts);
    if (opts[0] != Qundef) {
        ndf.func = <%=c_iter%>_nan;
    }
    <% else %>
    rb_scan_args(argc, argv, "20", &a1, &a2);
    <% end %>

    return na_ndloop(&ndf, 2, a1, a2);
}
