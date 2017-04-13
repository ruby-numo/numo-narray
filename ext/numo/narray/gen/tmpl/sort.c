<% (is_float ? ["","_nan"] : [""]).each do |j| %>
static void
<%=c_iter%><%=j%>(na_loop_t *const lp)
{
    size_t n;
    char *ptr;
    ssize_t step;

    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, ptr, step);
    <%=tp%>_qsort(ptr, n, step);
}
<% end %>

/*
 *  call-seq:
 *     narray.sort() => narray
 *     narray.sort(dim0,dim1,...) => narray
 *
 *  Return an index array of sort result.
 *
 *     Numo::DFloat[3,4,1,2].sort => Numo::DFloat[1,2,3,4]
 */
static VALUE
<%=c_func%>(int argc, VALUE *argv, VALUE self)
{
    int ignore_nan = 0;
    VALUE reduce;
    ndfunc_arg_in_t ain[2] = {{OVERWRITE,0},{sym_reduce,0}};
    ndfunc_t ndf = {<%=c_iter%>, STRIDE_LOOP|NDF_FLAT_REDUCE, 2,0, ain,0};

    if (!TEST_INPLACE(self)) {
        self = na_copy(self);
    }
    reduce = na_reduce_dimension(argc, argv, 1, &self, &ignore_nan); // v[0] = self
<% if is_float %>
    if (ignore_nan) {
        ndf.func = <%=c_iter%>_nan;
    }
<% end %>
    na_ndloop(&ndf, 2, self, reduce);
    return self;
}
