static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t   i;
    BIT_DIGIT *a1, x=0;
    size_t   p1;
    ssize_t  s1;
    size_t  *idx1;
    VALUE  y;

    INIT_COUNTER(lp, i);
    INIT_PTR_BIT_IDX(lp, 0, a1, p1, s1, idx1);
    if (idx1) {
        for (; i--;) {
            LOAD_BIT(a1, p1+*idx1, x); idx1++;
            y = m_data_to_num(x);
            rb_yield(y);
        }
    } else {
        for (; i--;) {
            LOAD_BIT(a1, p1, x); p1+=s1;
            y = m_data_to_num(x);
            rb_yield(y);
        }
    }
}

/*
  Calls the given block once for each element in self,
  passing that element as a parameter.
  @overload <%=name%>
  @return [Numo::NArray] self
  For a block {|x| ... }
  @yield [x]  x is element of NArray.
*/
static VALUE
<%=c_func(0)%>(VALUE self)
{
    ndfunc_arg_in_t ain[1] = {{Qnil,0}};
    ndfunc_t ndf = {<%=c_iter%>, FULL_LOOP_NIP, 1,0, ain,0};

    na_ndloop(&ndf, 1, self);
    return self;
}
