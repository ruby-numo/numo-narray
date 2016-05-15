static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t n;
    char *ptr;
    ssize_t step;

    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, ptr, step);
    <%=tp%>_qsort(ptr, n, step);
}

/*
 *  call-seq:
 *     narray.sort() => narray
 *     narray.sort(dim) => narray
 *
 *  Return an index array of sort result.
 *
 *     Numo::DFloat[3,4,1,2].sort_index => Numo::DFloat[1,2,3,4]
 */
static VALUE
<%=c_func%>(int argc, VALUE *argv, VALUE self)
{
    VALUE reduce;
    ndfunc_arg_in_t ain[2] = {{cT,0},{sym_reduce,0}};
    ndfunc_t ndf = {<%=c_iter%>, STRIDE_LOOP_NIP|NDF_FLAT_REDUCE, 2,0, ain,0};

    if (!TEST_INPLACE(self)) {
        self = na_copy(self);
    }
    reduce = na_reduce_dimension(argc, argv, 1, &self); // v[0] = self

    na_ndloop(&ndf, 2, self, reduce);
    return self;
}
