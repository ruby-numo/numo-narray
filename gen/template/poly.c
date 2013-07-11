static void
<%=c_iterator%>(na_loop_t *const lp)
{
    size_t  i;
    dtype  x, y, a;

    x = *(dtype*)(lp->args[0].ptr + lp->iter[0].pos);
    i = lp->narg - 2;
    y = *(dtype*)(lp->args[i].ptr + lp->iter[i].pos);
    for (; --i;) {
        y = m_mul(x,y);
        a = *(dtype*)(lp->args[i].ptr + lp->iter[i].pos);
        y = m_add(y,a);
    }
    i = lp->narg - 1;
    *(dtype*)(lp->args[i].ptr + lp->iter[i].pos) = y;
}

static VALUE
<%=c_instance_method%>(VALUE self, VALUE args)
{
    int argc, i;
    ndfunc_t *func;
    VALUE *types;
    VALUE *argv;
    volatile VALUE v, a;

    argc = RARRAY_LEN(args);
    types = ALLOCA_N(VALUE,argc+2);
    for (i=0; i<argc+2; i++) {
        types[i] = cT;
    }
    argv = ALLOCA_N(VALUE,argc+1);
    argv[0] = self;
    for (i=0; i<argc; i++) {
        argv[i+1] = RARRAY_PTR(args)[i];
    }
    a = rb_ary_new4(argc+1, argv);

    func = ndfunc_alloc2(<%=c_iterator%>, NO_LOOP,
                         argc+1, 1, types);
    v = ndloop_do2(func, a);
    ndfunc_free(func);
    return nary_<%=tp%>_extract(v);
}
