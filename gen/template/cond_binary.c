static void
<%=c_iterator%>(na_loop_t *const lp)
{
    size_t  i;
    char   *p1, *p2;
    BIT_DIGIT *a3;
    size_t  p3;
    ssize_t s1, s2, s3;
    size_t *idx1, *idx2, *idx3;
    dtype   x, y;
    BIT_DIGIT b;
    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2, idx2);
    INIT_PTR_BIT(lp, 2, a3, p3, s3, idx3);
    if (idx1||idx2||idx3) {
        for (; i--;) {
            LOAD_DATA_STEP(p1, s1, idx1, dtype, x);
            LOAD_DATA_STEP(p2, s2, idx2, dtype, y);
            b = (m_<%=op%>(x,y)) ? 1:0;
            STORE_BIT_STEP(a3, p3, s3, idx3, b);
        }
    } else {
        for (; i--;) {
            x = *(dtype*)p1;
            p1+=s1;
            y = *(dtype*)p2;
            p2+=s2;
            b = (m_<%=op%>(x,y)) ? 1:0;
            STORE_BIT(a3,p3,b)
            p3+=s3;
        }
    }
}

/*
  Class method <%=op%>.
  @overload <%=op%>(a1,a2)
  @param [NArray,Numeric] a1
  @param [NArray,Numeric] a2
  @return [NArray::Bit] <%=op%> of a1 and a2.
*/
static VALUE
<%=c_singleton_method%>(VALUE mod, VALUE a1, VALUE a2)
{
    ndfunc_t *func;
    VALUE v;
    func = ndfunc_alloc(<%=c_iterator%>, FULL_LOOP,
                        2, 1, cT, cT, cBit);
    v = ndloop_do(func, 2, a1, a2);
    ndfunc_free(func);
    return v;
}

/*
  Condition <%=op%>.
  @overload <%=op_map%> other
  @param [NArray,Numeric] other
  @return [NArray::Bit] result of self <%=op%> other.
*/
static VALUE
<%=c_instance_method%>(VALUE a1, VALUE a2)
{
    VALUE klass;
    klass = na_upcast(CLASS_OF(a1),CLASS_OF(a2));
    if (klass==cT) {
        return <%=c_singleton_method%>(cT,a1,a2);
    } else {
        return rb_funcall(klass,id_<%=op%>,2,a1,a2);
    }
}
