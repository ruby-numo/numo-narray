/*
  Returns two index arrays.
  The first array contains index where the bit is one (true).
  The second array contains index where the bit is zero (false).
  @overload <%=op_map%>
  @return [Numo::Int32,Numo::Int64]*2
*/
static VALUE
numo_bit_where2(VALUE self)
{
    VALUE idx_1, idx_0;
    size_t size, n_1, n_0;
    where_opt_t *g;

    ndfunc_arg_in_t ain[1] = {{cT,0}};
    ndfunc_t ndf = { iter_bit_where, FULL_LOOP, 1, 0, ain, 0 };

    size = RNARRAY_SIZE(self);
    n_1 = NUM2SIZET(numo_bit_count_true(0, NULL, self));
    n_0 = size - n_1;
    g = ALLOCA_N(where_opt_t,1);
    g->count = 0;
    if (size>4294967295ul) {
        idx_1 = rb_narray_new(numo_cInt64, 1, &n_1);
        idx_0 = rb_narray_new(numo_cInt64, 1, &n_0);
        g->elmsz = 8;
    } else {
        idx_1 = rb_narray_new(numo_cInt32, 1, &n_1);
        idx_0 = rb_narray_new(numo_cInt32, 1, &n_0);
        g->elmsz = 4;
    }
    g->idx1 = na_get_pointer_for_write(idx_1);
    g->idx0 = na_get_pointer_for_write(idx_0);
    na_ndloop3(&ndf, g, 1, self);
    na_release_lock(idx_0);
    na_release_lock(idx_1);
    return rb_assoc_new(idx_1,idx_0);
}
