static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t     i;
    BIT_DIGIT *a1, *a2;
    size_t     p1,  p2;
    ssize_t    s1,  s2;
    size_t    *idx1, *idx2;
    BIT_DIGIT  x=0, y=0;

    INIT_COUNTER(lp, i);
    INIT_PTR_BIT_IDX(lp, 0, a1, p1, s1, idx1);
    INIT_PTR_BIT_IDX(lp, 1, a2, p2, s2, idx2);
    if (idx2) {
        if (idx1) {
            for (; i--;) {
                LOAD_BIT(a1, p1+*idx1, x); idx1++;
                LOAD_BIT(a2, p2+*idx2, y);
                x = m_<%=method%>(x,y);
                if (x != <%=init_bit%>) {
                    STORE_BIT(a2, p2+*idx2, x);
                    return;
                }
                idx2++;
            }
        } else {
            for (; i--;) {
                LOAD_BIT(a1, p1, x); p1+=s1;
                LOAD_BIT(a2, p2+*idx2, y);
                if (x != <%=init_bit%>) {
                    STORE_BIT(a2, p2+*idx2, x);
                    return;
                }
                idx2++;
            }
        }
    } else if (s2) {
        if (idx1) {
            for (; i--;) {
                LOAD_BIT(a1, p1+*idx1, x); idx1++;
                LOAD_BIT(a2, p2, y);
                x = m_<%=method%>(x,y);
                if (x != <%=init_bit%>) {
                    STORE_BIT(a2, p2, x);
                    return;
                }
                p2 += s2;
            }
        } else {
            for (; i--;) {
                LOAD_BIT(a1, p1, x); p1+=s1;
                LOAD_BIT(a2, p2, y);
                if (x != <%=init_bit%>) {
                    STORE_BIT(a2, p2, x);
                    return;
                }
                p2 += s2;
            }
        }
    } else {
        LOAD_BIT(a2, p2, x);
        if (x != <%=init_bit%>) {
            return;
        }
        if (idx1) {
            for (; i--;) {
                LOAD_BIT(a1, p1+*idx1, x); idx1++;
                if (x != <%=init_bit%>) {
                    STORE_BIT(a2, p2, x);
                    return;
                }
            }
        } else {
            for (; i--;) {
                LOAD_BIT(a1, p1, x); p1+=s1;
                if (x != <%=init_bit%>) {
                    STORE_BIT(a2, p2, x);
                    return;
                }
            }
        }
    }
}

/*
  <%=method%>.
  @overload <%=method%>
  @return [Numo::Int64] the number of true bits.
*/
static VALUE
<%=c_func%>(int argc, VALUE *argv, VALUE self)
{
    VALUE v, reduce;
    ndfunc_arg_in_t ain[3] = {{cT,0},{sym_reduce,0},{sym_init,0}};
    ndfunc_arg_out_t aout[1] = {{numo_cBit,0}};
    ndfunc_t ndf = {<%=c_iter%>, FULL_LOOP_NIP, 3,1, ain,aout};

    reduce = na_reduce_dimension(argc, argv, 1, &self);
    v = na_ndloop(&ndf, 3, self, reduce, INT2FIX(<%=init_bit%>));
    return rb_funcall(v,rb_intern("extract"),0);
    //return v;
}
