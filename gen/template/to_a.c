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
    ndfunc_arg_in_t ain[3] = {{Qnil,0},{sym_loop_opt},{sym_option}};
    ndfunc_arg_out_t aout[1] = {{rb_cArray,0}}; // dummy?
    ndfunc_t ndf = { <%=c_iterator%>, FULL_LOOP, 3, 1, ain, aout };
    return na_ndloop_cast_narray_to_rarray(&ndf, self, Qnil);
}
