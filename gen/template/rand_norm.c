static void
<%=c_iterator%>(na_loop_t *const lp)
{
    size_t   i;
    char    *p1;
    ssize_t  s1;
    size_t  *idx1;
    dtype    y;
    dtype    a[2];

    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1, idx1);
    if (idx1) {
        for (; i>1; i-=2) {
            m_rand_norm(a);
            *(dtype*)(p1+*idx1)     = a[0];
            *(dtype*)(p1+*(idx1+1)) = a[1];
            idx1 += 2;
        }
        if (i>0) {
            m_rand_norm(a);
            *(dtype*)(p1+*idx1) = a[0];
        }
    } else {
        for (; i>1; i-=2) {
            m_rand_norm(a);
            *(dtype*)(p1)    = a[0];
            *(dtype*)(p1+s1) = a[1];
            p1 += s1*2;
        }
        if (i>0) {
            m_rand_norm(a);
            *(dtype*)(p1) = a[0];
        }
    }
}

static VALUE
<%=c_instance_method%>(VALUE self)
{
    ndfunc_t *func;
    func = ndfunc_alloc(<%=c_iterator%>, FULL_LOOP,
                        1, 0, Qnil);
    ndloop_do(func, 1, self);
    ndfunc_free(func);
    return self;
}
