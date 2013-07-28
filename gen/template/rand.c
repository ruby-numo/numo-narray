static void
<%=c_iterator%>(na_loop_t *const lp)
{
    size_t   i;
    char    *p1;
    ssize_t  s1;
    size_t  *idx1;
    dtype    y;

    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1, idx1);
    if (idx1) {
        for (; i--;) {
            y = m_rand;
            *(dtype*)(p1+*idx1) = y;
            idx1++;
        }
    } else {
        for (; i--;) {
            y = m_rand;
            *(dtype*)(p1) = y;
            p1+=s1;
        }
    }
}

static VALUE
<%=c_instance_method%>(VALUE self)
{
    ndfunc_arg_in_t ain[1] = {{Qnil,0}};
    ndfunc_t ndf = { <%=c_iterator%>, FULL_LOOP, 1, 0, ain, 0 };

    na_ndloop(&ndf, 1, self);
    return self;
}
