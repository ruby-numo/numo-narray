static void
<%=c_iterator%>(na_loop_t *const lp)
{
    size_t  i;
    char    *p1, *p2;
    dtype   *q2;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    dtype   x, y;

    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2, idx2);
    if (idx2==0 && s2==0) {
        y = *(dtype*)p2;
        if (idx1) {
            for (; i--;) {
                x = *(dtype*)(p1 + *idx1);
                idx1++;
                m_<%=op%>(x,y);
            }
        } else {
            for (; i--;) {
                x = *(dtype*)p1;
                p1+=s1;
                m_<%=op%>(x,y);
            }
        }
        *(dtype*)p2 = y;
    } else {
        if (idx1||idx2) {
            for (; i--;) {
                LOAD_DATA_STEP(p1, s1, idx1, dtype, x);
                q2 = (dtype*)p2;
                LOAD_DATA_STEP(p2, s2, idx2, dtype, y);
                m_<%=op%>(x,y);
                *q2 = y;
            }
        } else {
            for (; i--;) {
                x = *(dtype*)p1;
                p1+=s1;
                y = *(dtype*)p2;
                m_<%=op%>(x,y);
                *(dtype*)p2 = y;
                p2+=s2;
            }
        }
    }
}

/*
  <%=op.capitalize%> of self.
  @overload <%=op%>(*args)
  @param [Array of Numeric,Range] args  Affected dimensions.
  @return [NArray::<%=class_name%>] <%=op%> of self.
*/
static VALUE
<%=c_instance_method%>(int argc, VALUE *argv, VALUE self)
{
    VALUE v, accum;
    ndfunc_t *func;
    func = ndfunc_alloc(<%=c_iterator%>, FULL_LOOP,
                        2, 1, cT, sym_reduce, cT);
    accum = na_reduce_dimension(argc, argv, self);
    func->args[1].init = m_<%=op%>_init;
    v = ndloop_do(func, 2, self, accum);
    ndfunc_free(func);
    return nary_<%=tp%>_extract(v);
}
