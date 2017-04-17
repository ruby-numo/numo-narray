static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t  i;
    BIT_DIGIT *a;
    size_t  p;
    ssize_t s;
    size_t *idx;
    BIT_DIGIT x=0;
    char   *idx0, *idx1;
    size_t  count;
    size_t  e;
    where_opt_t *g;

    g = (where_opt_t*)(lp->opt_ptr);
    count = g->count;
    idx0  = g->idx0;
    idx1  = g->idx1;
    e     = g->elmsz;
    INIT_COUNTER(lp, i);
    INIT_PTR_BIT_IDX(lp, 0, a, p, s, idx);
    if (idx) {
        for (; i--;) {
            LOAD_BIT(a, p+*idx, x);
            idx++;
            if (x==0) {
                STORE_INT(idx0,e,count);
                idx0 += e;
            } else {
                STORE_INT(idx1,e,count);
                idx1 += e;
            }
            count++;
        }
    } else {
        for (; i--;) {
            LOAD_BIT(a, p, x);
            p+=s;
            if (x==0) {
                STORE_INT(idx0,e,count);
                idx0 += e;
            } else {
                STORE_INT(idx1,e,count);
                idx1 += e;
            }
            count++;
        }
    }
    g->count = count;
    g->idx0  = idx0;
    g->idx1  = idx1;
}

/*
  Returns two index arrays.
  The first array contains index where the bit is one (true).
  The second array contains index where the bit is zero (false).
  @overload <%=op_map%>
  @return [Numo::Int32,Numo::Int64]*2
*/
static VALUE
<%=c_func(0)%>(VALUE self)
{
    VALUE idx_1, idx_0;
    size_t size, n_1, n_0;
    where_opt_t *g;

    ndfunc_arg_in_t ain[1] = {{cT,0}};
    ndfunc_t ndf = { <%=c_iter%>, FULL_LOOP, 1, 0, ain, 0 };

    size = RNARRAY_SIZE(self);
    n_1 = NUM2SIZET(<%=find_tmpl("count_true").c_func%>(0, NULL, self));
    n_0 = size - n_1;
    g = ALLOCA_N(where_opt_t,1);
    g->count = 0;
    if (size>4294967295ul) {
        idx_1 = nary_new(numo_cInt64, 1, &n_1);
        idx_0 = nary_new(numo_cInt64, 1, &n_0);
        g->elmsz = 8;
    } else {
        idx_1 = nary_new(numo_cInt32, 1, &n_1);
        idx_0 = nary_new(numo_cInt32, 1, &n_0);
        g->elmsz = 4;
    }
    g->idx1 = na_get_pointer_for_write(idx_1);
    g->idx0 = na_get_pointer_for_write(idx_0);
    na_ndloop3(&ndf, g, 1, self);
    na_release_lock(idx_0);
    na_release_lock(idx_1);
    return rb_assoc_new(idx_1,idx_0);
}
