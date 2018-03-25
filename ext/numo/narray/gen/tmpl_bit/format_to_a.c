static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t  i;
    BIT_DIGIT *a1, x=0;
    size_t     p1;
    ssize_t    s1;
    size_t   *idx1;
    VALUE y;
    VALUE fmt = lp->option;
    volatile VALUE a;

    INIT_COUNTER(lp, i);
    INIT_PTR_BIT_IDX(lp, 0, a1, p1, s1, idx1);
    a = rb_ary_new2(i);
    rb_ary_push(lp->args[1].value, a);
    if (idx1) {
        for (; i--;) {
            LOAD_BIT(a1, p1+*idx1, x); idx1++;
            y = format_bit(fmt, x);
            rb_ary_push(a,y);
        }
    } else {
        for (; i--;) {
            LOAD_BIT(a1, p1, x); p1+=s1;
            y = format_bit(fmt, x);
            rb_ary_push(a,y);
        }
    }
}

/*
  Format elements into strings.
  @overload <%=name%> format
  @param [String] format
  @return [Array] array of formated strings.
*/
static VALUE
<%=c_func(-1)%>(int argc, VALUE *argv, VALUE self)
{
    VALUE fmt=Qnil;
    ndfunc_arg_in_t ain[3] = {{Qnil,0},{sym_loop_opt},{sym_option}};
    ndfunc_arg_out_t aout[1] = {{rb_cArray,0}}; // dummy?
    ndfunc_t ndf = {<%=c_iter%>, FULL_LOOP_NIP, 3,1, ain,aout};

    rb_scan_args(argc, argv, "01", &fmt);
    return na_ndloop_cast_narray_to_rarray(&ndf, self, fmt);
}
