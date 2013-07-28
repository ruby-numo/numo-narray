static void
<%=c_iterator%>(na_loop_t *const lp)
{
    size_t  i;
    char   *p1, *p2;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    dtype   x;
    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2, idx2);
    if (idx1||idx2) {
        for (; i--;) {
            LOAD_DATA_STEP(p1, s1, idx1, dtype, x);
            x = m_<%=op%>(x);
            STORE_DATA_STEP(p2, s2, idx2, dtype, x);
        }
    } else {
        for (; i--;) {
            x = *(dtype*)p1;
            p1+=s1;
            x = m_<%=op%>(x);
            *(dtype*)p2 = x;
            p2+=s2;
        }
    }
}

/*
  Calculate <%=op%>(x).
  @overload <%=op%>(x)
  @param [NArray,Numeric] x  input value
  @return [NArray::<%=class_name%>] result of <%=op%>(x).
*/
static VALUE
<%=c_singleton_method%>(VALUE mod, VALUE a1)
{
    ndfunc_arg_in_t ain[1] = {{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { <%=c_iterator%>, FULL_LOOP, 1, 1, ain, aout };

    return na_ndloop(&ndf, 1, a1);
}
