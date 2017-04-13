<% (is_float ? ["","_nan"] : [""]).each do |j| %>
static void
<%=c_iter%><%=j%>(na_loop_t *const lp)
{
    size_t n;
    char *p1, *p2;
    dtype *buf;

    INIT_COUNTER(lp, n);
    p1 = (lp->args[0]).ptr + (lp->args[0].iter[0]).pos;
    p2 = (lp->args[1]).ptr + (lp->args[1].iter[0]).pos;
    buf = (dtype*)p1;

    <%=tp%>_qsort(buf, n, sizeof(dtype));

    <% if is_float %>
    for (; n; n--) {
        if (!isnan(buf[n-1])) break;
    }
    <% end %>

    if (n==0) {
        *(dtype*)p2 = buf[0];
    }
    else if (n%2==0) {
        *(dtype*)p2 = (buf[n/2-1]+buf[n/2])/2;
    }
    else {
        *(dtype*)p2 = buf[(n-1)/2];
    }
}
<% end %>

static VALUE
<%=c_func%>(int argc, VALUE *argv, VALUE self)
{
    int ignore_nan = 0;
    VALUE reduce;
    ndfunc_arg_in_t ain[2] = {{OVERWRITE,0},{sym_reduce,0}};
    ndfunc_arg_out_t aout[1] = {{INT2FIX(0),0}};
    ndfunc_t ndf = {<%=c_iter%>, NDF_HAS_LOOP|NDF_FLAT_REDUCE, 2,1, ain,aout};

    self = na_copy(self); // as temporary buffer
    reduce = na_reduce_dimension(argc, argv, 1, &self, &ignore_nan); // v[0] = self
<% if is_float %>
    if (ignore_nan) {
        ndf.func = <%=c_iter%>_nan;
    }
<% end %>
    return na_ndloop(&ndf, 2, self, reduce);
}
