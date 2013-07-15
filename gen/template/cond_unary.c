static void
<%=c_iterator%>(na_loop_t *const lp)
{
    size_t   i;
    char    *p1;
    BIT_DIGIT *a2;
    size_t   p2;
    ssize_t  s1, s2;
    size_t *idx1, *idx2;
    dtype    x;
    BIT_DIGIT b;
    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1, idx1);
    INIT_PTR_BIT(lp, 1, a2, p2, s2, idx2);
    if (idx1||idx2) {
        for (; i--;) {
            LOAD_DATA_STEP(p1, s1, idx1, dtype, x);
            b = (m_<%=op%>(x)) ? 1:0;
            STORE_BIT_STEP(a2, p2, s2, idx2, b);
        }
    } else {
        for (; i--;) {
            x = *(dtype*)p1;
            p1+=s1;
            b = (m_<%=op%>(x)) ? 1:0;
            STORE_BIT(a2,p2,b)
            p2+=s2;
        }
    }
}

/*
  Condition of <%=op%>.
  @overload <%=op%>
  @return [NArray::Bit] Condition of <%=op%>.
*/
static VALUE
<%=c_instance_method%>(VALUE a1)
{
    ndfunc_t *func;
    VALUE v;
    func = ndfunc_alloc(<%=c_iterator%>, FULL_LOOP,
                        1, 1, cT, cBit);
    v = ndloop_do(func, 1, a1);
    ndfunc_free(func);
    return v;
}
