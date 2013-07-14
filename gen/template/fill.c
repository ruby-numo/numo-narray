static void
<%=c_iterator%>(na_loop_t *const lp)
{
    size_t   i;
    char    *p1;
    ssize_t  s1;
    size_t *idx1;
    VALUE    x = *(VALUE*)(lp->opt_ptr);
    dtype    y;
    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1, idx1);
    y = m_num_to_data(x);
    if (idx1) {
        for (; i--;) {*(dtype*)(p1+*(idx1++)) = y;}
    } else {
        for (; i--;) {*(dtype*)(p1) = y; p1+=s1;}
    }
}

/*
  Fill elements with other.
  @overload <%=op_map%> other
  @param [Numeric] other
  @return [NArray::<%=class_name%>] self.
*/
static VALUE
<%=c_instance_method%>(VALUE self, VALUE val)
{
    ndfunc_t *func;
    func = ndfunc_alloc(<%=c_iterator%>, FULL_LOOP,
                        1, 0, Qnil);
    ndloop_do3(func, &val, 1, self);
    ndfunc_free(func);
    return self;
}
