static void
<%=c_iterator%>_to_a(na_loop_t *const lp)
{
    size_t  i;
    char   *p1, *p2;
    ssize_t s1, s2;
    ssize_t *idx1, *idx2;
    void *x;
    VALUE y;
    volatile VALUE a;
    VALUE fmt = *(VALUE*)(lp->opt_ptr);
    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1, idx1);
    a = rb_ary_new2(i);
    rb_ary_push(lp->args[1].value, a);
    for (; i--;) {
        LOAD_PTR_STEP(p1, s1, idx1, void, x);
        y = format_<%=tp%>(fmt, x);
        rb_ary_push(a,y);
    }
}

static VALUE
<%=c_instance_method%>_to_a(int argc, VALUE *argv, VALUE self)
{
     ndfunc_t *func;
     volatile VALUE v, fmt=Qnil;

     rb_scan_args(argc, argv, "01", &fmt);
     func = ndfunc_alloc(<%=c_iterator%>_to_a,FULL_LOOP,
                         1, 1, Qnil, rb_cArray);
     v = ndloop_cast_narray_to_rarray(func, self, fmt);
     ndfunc_free(func);
     return v;
}
