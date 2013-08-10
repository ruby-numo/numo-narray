#undef int_t
#define int_t int64_t

static void
<%=c_iterator%>(na_loop_t *const lp)
{
    size_t  i;
    BIT_DIGIT *a1;
    size_t  p1;
    char   *p2;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    BIT_DIGIT x=0;
    int_t   y;

    INIT_COUNTER(lp, i);
    INIT_PTR_BIT(lp, 0, a1, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);
    if (idx2==0 && s2==0) {
        y = *(int_t*)p2;
        if (idx1) {
            for (; i--;) {
                LOAD_BIT(a1, p1+*idx1, x);
                idx1++;
                if (m_<%=op%>(x)) {
                    y++;
                }
            }
        } else {
            for (; i--;) {
                LOAD_BIT(a1, p1, x);
                p1 += s1;
                if (m_<%=op%>(x)) {
                    y++;
                }
            }
        }
        *(int_t*)p2 = y;
    } else if (idx1||idx2) {
        for (; i--;) {
            LOAD_BIT_STEP(a1, p1, s1, idx1, x);
            LOAD_DATA_NOSTEP(p2, s2, idx2, int_t, y);
            if (m_<%=op%>(x)) {
                y++;
            }
            STORE_DATA_STEP(p2, s2, idx2, int_t, y);
        }
    } else {
        for (; i--;) {
            LOAD_BIT(a1, p1, x);
            p1+=s1;
            y = *(int_t*)p2;
            if (m_<%=op%>(x)) {
                y++;
                *(int_t*)p2 = y;
            }
            p2+=s2;
        }
    }
}

/*
  <%=op%>.
  @overload <%=op%>
  @return [NArray::Int64] the number of true bits.
*/
static VALUE
<%=c_instance_method%>(int argc, VALUE *argv, VALUE self)
{
    VALUE v, reduce;
    ndfunc_arg_in_t ain[3] = {{cT,0},{sym_reduce,0},{sym_init,0}};
    ndfunc_arg_out_t aout[1] = {{cInt64,0}};
    ndfunc_t ndf = { <%=c_iterator%>, FULL_LOOP, 3, 1, ain, aout };

    reduce = na_reduce_dimension(argc, argv, self);
    v = na_ndloop(&ndf, 3, self, reduce, INT2FIX(0));
    return rb_funcall(v,rb_intern("extract"),0);
    //return nary_int64_extract(v);
}
