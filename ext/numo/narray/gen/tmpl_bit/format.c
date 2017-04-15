static VALUE
format_<%=type_name%>(VALUE fmt, dtype x)
{
    if (NIL_P(fmt)) {
        char s[4];
        int n;
        n = m_sprintf(s,x);
        return rb_str_new(s,n);
    }
    return rb_funcall(fmt, '%', 1, m_data_to_num(x));
}

static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t  i;
    BIT_DIGIT *a1, x=0;
    size_t     p1;
    char      *p2;
    ssize_t    s1, s2;
    size_t    *idx1;
    VALUE  y;
    VALUE  fmt = lp->option;

    INIT_COUNTER(lp, i);
    INIT_PTR_BIT_IDX(lp, 0, a1, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2);

    if (idx1) {
        for (; i--;) {
            LOAD_BIT(a1, p1+*idx1, x); idx1++;
            y = format_<%=type_name%>(fmt, x);
            SET_DATA_STRIDE(p2, s2, VALUE, y);
        }
    } else {
        for (; i--;) {
            LOAD_BIT(a1, p1, x); p1+=s1;
            y = format_<%=type_name%>(fmt, x);
            SET_DATA_STRIDE(p2, s2, VALUE, y);
        }
    }
}

/*
  Format elements into strings.
  @overload <%=name%> format
  @param [String] format
  @return [Numo::RObject] array of formated strings.
*/
static VALUE
<%=c_func(-1)%>(int argc, VALUE *argv, VALUE self)
{
    VALUE fmt=Qnil;

    ndfunc_arg_in_t ain[2] = {{Qnil,0},{sym_option}};
    ndfunc_arg_out_t aout[1] = {{numo_cRObject,0}};
    ndfunc_t ndf = {<%=c_iter%>, FULL_LOOP_NIP, 2,1, ain,aout};

    rb_scan_args(argc, argv, "01", &fmt);
    return na_ndloop(&ndf, 2, self, fmt);
}
