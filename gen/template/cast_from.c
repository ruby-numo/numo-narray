static void
<%=c_iterator%>(na_loop_t *const lp)
{
    size_t  i, s1, s2;
    char   *p1, *p2;
    size_t *idx1, *idx2;
    <%=dtype%> x;
    dtype y;

    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2, idx2);
    if (idx1||idx2) {
        for (; i--;) {
            LOAD_DATA_STEP(p1, s2, idx1, <%=dtype%>, x);
            y = <%=macro%>(x);
            STORE_DATA_STEP(p2, s2, idx2, dtype, y);
        }
    } else {
        for (; i--;) {
            x = *(<%=dtype%>*)p1;
            p1+=s1;
            y = <%=macro%>(x);
            *(dtype*)p2 = y;
            p2+=s2;
        }
    }
}

static VALUE
<%=c_function%>(VALUE obj)
{
    VALUE v;
    ndfunc_t *func;
    func = ndfunc_alloc(<%=c_iterator%>, FULL_LOOP,
                        1, 1, Qnil, cT);
    v = ndloop_do(func, 1, obj);
    ndfunc_free(func);
    return v;
}
