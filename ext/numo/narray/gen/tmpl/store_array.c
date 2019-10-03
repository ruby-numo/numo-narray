static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t i, n;
    size_t i1, n1;
    VALUE  v1, *ptr;
    char   *p1;
    size_t s1, *idx1;
    VALUE  x;
    double y;
    dtype  z;
    size_t len, c;
    double beg, step;

    INIT_COUNTER(lp, n);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    v1 = lp->args[1].value;
    i = 0;

    if (lp->args[1].ptr) {
        if (v1 == Qtrue) {
            iter_<%=type_name%>_store_<%=type_name%>(lp);
            i = lp->args[1].shape[0];
            if (idx1) {
                idx1 += i;
            } else {
                p1 += s1 * i;
            }
        }
        goto loop_end;
    }

    ptr = &v1;

    switch(TYPE(v1)) {
    case T_ARRAY:
        n1 = RARRAY_LEN(v1);
        ptr = RARRAY_PTR(v1);
        break;
    case T_NIL:
        n1 = 0;
        break;
    default:
        n1 = 1;
    }

    if (idx1) {
        for (i=i1=0; i1<n1 && i<n; i++,i1++) {
            x = ptr[i1];
            if (rb_obj_is_kind_of(x, rb_cRange)
#ifdef HAVE_RB_ARITHMETIC_SEQUENCE_EXTRACT
                || rb_obj_is_kind_of(x, rb_cArithSeq)
#else
                || rb_obj_is_kind_of(x, rb_cEnumerator)
#endif
                ) {
                nary_step_sequence(x,&len,&beg,&step);
                for (c=0; c<len && i<n; c++,i++) {
                    y = beg + step * c;
                    z = m_from_double(y);
                    SET_DATA_INDEX(p1, idx1, dtype, z);
                }
            }
            else if (TYPE(x) != T_ARRAY) {
                z = m_num_to_data(x);
                SET_DATA_INDEX(p1, idx1, dtype, z);
            }
        }
    } else {
        for (i=i1=0; i1<n1 && i<n; i++,i1++) {
            x = ptr[i1];
            if (rb_obj_is_kind_of(x, rb_cRange)
#ifdef HAVE_RB_ARITHMETIC_SEQUENCE_EXTRACT
                || rb_obj_is_kind_of(x, rb_cArithSeq)
#else
                || rb_obj_is_kind_of(x, rb_cEnumerator)
#endif
                ) {
                nary_step_sequence(x,&len,&beg,&step);
                for (c=0; c<len && i<n; c++,i++) {
                    y = beg + step * c;
                    z = m_from_double(y);
                    SET_DATA_STRIDE(p1, s1, dtype, z);
                }
            }
            else if (TYPE(x) != T_ARRAY) {
                z = m_num_to_data(x);
                SET_DATA_STRIDE(p1, s1, dtype, z);
            }
        }
    }

 loop_end:
    z = m_zero;
    if (idx1) {
        for (; i<n; i++) {
            SET_DATA_INDEX(p1, idx1, dtype, z);
        }
    } else {
        for (; i<n; i++) {
            SET_DATA_STRIDE(p1, s1, dtype, z);
        }
    }
}

static VALUE
<%=c_func%>(VALUE self, VALUE rary)
{
    ndfunc_arg_in_t ain[2] = {{OVERWRITE,0},{rb_cArray,0}};
    ndfunc_t ndf = {<%=c_iter%>, FULL_LOOP, 2, 0, ain, 0};

    na_ndloop_store_rarray(&ndf, self, rary);
    return self;
}
