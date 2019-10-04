static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t  i;
    char   *p1, *p2, *p3, *p4;
    ssize_t s1, s2, s3, s4;
    dtype   x, min, max;
    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR(lp, 2, p3, s3);
    INIT_PTR(lp, 3, p4, s4);
    for (; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,dtype,min);
        GET_DATA_STRIDE(p3,s3,dtype,max);
        if (m_gt(min,max)) {rb_raise(nary_eOperationError,"min is greater than max");}
        if (m_lt(x,min)) {x=min;}
        if (m_gt(x,max)) {x=max;}
        SET_DATA_STRIDE(p4,s4,dtype,x);
    }
}

static void
<%=c_iter%>_min(na_loop_t *const lp)
{
    size_t  i;
    char   *p1, *p2, *p3;
    ssize_t s1, s2, s3;
    dtype   x, min;
    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR(lp, 2, p3, s3);
    for (; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,dtype,min);
        if (m_lt(x,min)) {x=min;}
        SET_DATA_STRIDE(p3,s3,dtype,x);
    }
}

static void
<%=c_iter%>_max(na_loop_t *const lp)
{
    size_t  i;
    char   *p1, *p2, *p3;
    ssize_t s1, s2, s3;
    dtype   x, max;
    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR(lp, 2, p3, s3);
    for (; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,dtype,max);
        if (m_gt(x,max)) {x=max;}
        SET_DATA_STRIDE(p3,s3,dtype,x);
    }
}

/*
  Clip array elements by [min,max].
  If either of min or max is nil, one side is clipped.
  @overload <%=name%>(min,max)
  @param [Numo::NArray,Numeric] min
  @param [Numo::NArray,Numeric] max
  @return [Numo::NArray] result of clip.

  @example
      a = Numo::Int32.new(10).seq
      # => Numo::Int32#shape=[10]
      # [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

      a.clip(1,8)
      # => Numo::Int32#shape=[10]
      # [1, 1, 2, 3, 4, 5, 6, 7, 8, 8]

      a.inplace.clip(3,6)
      a
      # => Numo::Int32#shape=[10]
      # [3, 3, 3, 3, 4, 5, 6, 6, 6, 6]

      b = Numo::Int32.new(10).seq
      # => Numo::Int32#shape=[10]
      # [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

      b.clip([3,4,1,1,1,4,4,4,4,4], 8)
      # => Numo::Int32#shape=[10]
      # [3, 4, 2, 3, 4, 5, 6, 7, 8, 8]
*/
static VALUE
<%=c_func(2)%>(VALUE self, VALUE min, VALUE max)
{
    ndfunc_arg_in_t ain[3] = {{Qnil,0},{cT,0},{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf_min = { <%=c_iter%>_min, STRIDE_LOOP, 2, 1, ain, aout };
    ndfunc_t ndf_max = { <%=c_iter%>_max, STRIDE_LOOP, 2, 1, ain, aout };
    ndfunc_t ndf_both = { <%=c_iter%>, STRIDE_LOOP, 3, 1, ain, aout };

    if (RTEST(min)) {
        if (RTEST(max)) {
            return na_ndloop(&ndf_both, 3, self, min, max);
        } else {
            return na_ndloop(&ndf_min, 2, self, min);
        }
    } else {
        if (RTEST(max)) {
            return na_ndloop(&ndf_max, 2, self, max);
        }
    }
    rb_raise(rb_eArgError,"min and max are not given");
    return Qnil;
}
