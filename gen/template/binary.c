static void
<%=c_iterator%>(na_loop_t *const lp)
{
    size_t   i, n;
    char    *p1, *p2, *p3;
    ssize_t  s1, s2, s3;
    size_t  *idx1, *idx2, *idx3;
    dtype    x, y;
    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2, idx2);
    INIT_PTR(lp, 2, p3, s3, idx3);
    for (i=n; i--;) {
        x = *(dtype*)p1; p1+=s1;
        y = *(dtype*)p2; p2+=s2;
        x = m_<%=op%>(x,y);
        *(dtype*)p3 = x; p3+=s3;
    }
}

static VALUE
<%=c_instance_method%>_self(VALUE self, VALUE other)
{
    ndfunc_arg_in_t ain[2] = {{cT,0},{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { <%=c_iterator%>, STRIDE_LOOP, 2, 1, ain, aout };

    return na_ndloop(&ndf, 2, self, other);
}

/*
  Binary <%=op%>.
  @overload <%=op_map%> other
  @param [NArray,Numeric] other
  @return [NArray] <%=op%> of self and other.
*/
static VALUE
<%=c_instance_method%>(VALUE self, VALUE other)
{
    VALUE klass, v;
    klass = na_upcast(CLASS_OF(self),CLASS_OF(other));
    if (klass==cT) {
        return <%=c_instance_method%>_self(self, other);
    } else {
        v = rb_funcall(klass, id_cast, 1, self);
        return rb_funcall(v, <%=id_op%>, 1, other);
    }
}
