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
    if (idx1) {
        if (idx2) {
            for (; i--;) {
                x = *(<%=dtype%>*)(p1 + *idx1);
                idx1++;
                y = <%=macro%>(x);
                *(dtype*)(p2 + *idx2) = y;
                idx2++;
            }
        } else {
            for (; i--;) {
                x = *(<%=dtype%>*)(p1 + *idx1);
                idx1++;
                y = <%=macro%>(x);
                *(dtype*)p2 = y;
                p2+=s2;
            }
        }
    } else {
        if (idx2) {
            for (; i--;) {
                x = *(<%=dtype%>*)p1;
                p1+=s1;
                y = <%=macro%>(x);
                *(dtype*)(p2 + *idx2) = y;
                idx2++;
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
}

static VALUE
<%=c_function%>(VALUE obj)
{
    ndfunc_arg_in_t ain[1] = {{Qnil,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { <%=c_iterator%>, FULL_LOOP, 1, 1, ain, aout };

    return na_ndloop(&ndf, 1, obj);
}
