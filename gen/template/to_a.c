void
<%=c_iterator%>(na_loop_t *const lp)
{
    size_t i, s1;
    char *p1;
    size_t *idx1;
    dtype x;
    volatile VALUE a, y;

    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1, idx1);
    a = rb_ary_new2(i);
    rb_ary_push(lp->args[1].value, a);
    for (; i--;) {
        LOAD_DATA_STEP(p1, s1, idx1, dtype, x);
        y = m_data_to_num(x);
        rb_ary_push(a,y);
    }
}

/*
  Convert self to Array.
  @overload <%=op%>
  @return [Array]
*/
static VALUE
<%=c_instance_method%>(VALUE self)
{
    VALUE v;
    ndfunc_t *func ;

    func = ndfunc_alloc(<%=c_iterator%>, FULL_LOOP,
                        1, 1, Qnil, rb_cArray);
    v = ndloop_cast_narray_to_rarray(func, self, Qnil);
    ndfunc_free(func);
    return v;
}
