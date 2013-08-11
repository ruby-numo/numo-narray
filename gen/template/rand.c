static void
<%=c_iterator%>(na_loop_t *const lp)
{
    size_t   i;
    char    *p1;
    ssize_t  s1;
    size_t  *idx1;
    dtype    x;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    if (idx1) {
        for (; i--;) {
            x = m_rand;
            SET_DATA_INDEX(p1,idx1,dtype,x);
        }
    } else {
        for (; i--;) {
            x = m_rand;
            SET_DATA_INDEX(p1,idx1,dtype,x);
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
