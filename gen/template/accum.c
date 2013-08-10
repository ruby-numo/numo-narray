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
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);
    if (idx2==0 && s2==0) {
        // Reduce loop
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
                q2 = (dtype*)p2;
                LOAD_DATA_STEP(p1, s1, idx1, dtype, x);
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
    VALUE v, reduce;
    ndfunc_arg_in_t ain[3] = {{cT,0},{sym_reduce,0},{sym_init,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { <%=c_iterator%>, FULL_LOOP, 3, 1, ain, aout };

    reduce = na_reduce_dimension(argc, argv, self);
    v =  na_ndloop(&ndf, 3, self, reduce, m_<%=op%>_init);
    return nary_<%=tp%>_extract(v);
}
