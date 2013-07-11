static VALUE
format_<%=tp%>(VALUE fmt, void *x)
{
    // fix-me
    char s[48];
    int n;

    if (NIL_P(fmt)) {
        n = m_sprintf(s,*(dtype*)x);
        return rb_str_new(s,n);
    }
    return rb_funcall(fmt, '%', 1, m_data_to_num(*(dtype*)x));
}

static void
<%=c_iterator%>(na_loop_t *const lp)
{
    size_t  i;
    char   *p1, *p2;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    void *x;
    VALUE y;
    VALUE fmt = *(VALUE*)(lp->opt_ptr);
    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2, idx2);
    for (; i--;) {
        LOAD_PTR_STEP(p1, s1, idx1, void, x);
        y = format_<%=tp%>(fmt, x);
        STORE_DATA_STEP(p2, s2, idx2, VALUE, y);
    }
}

static VALUE
<%=c_instance_method%>(int argc, VALUE *argv, VALUE self)
{
     ndfunc_t *func;
     volatile VALUE v;
     VALUE fmt=Qnil;

     rb_scan_args(argc, argv, "01", &fmt);
     func = ndfunc_alloc(<%=c_iterator%>, FULL_LOOP,
                         1, 1, Qnil, cRObject);
     v = ndloop_do3(func, &fmt, 1, self);
     ndfunc_free(func);
     return v;
}
