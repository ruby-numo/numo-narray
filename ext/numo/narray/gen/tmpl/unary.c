static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t  i, n;
    char   *p1, *p2;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    dtype   x;

    INIT_COUNTER(lp, n);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);

    if (idx1) {
        if (idx2) {
            for (i=0; i<n; i++) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                x = m_<%=name%>(x);
                SET_DATA_INDEX(p2,idx2,dtype,x);
            }
        } else {
            for (i=0; i<n; i++) {
                GET_DATA_INDEX(p1,idx1,dtype,x);
                x = m_<%=name%>(x);
                SET_DATA_STRIDE(p2,s2,dtype,x);
            }
        }
    } else {
        if (idx2) {
            for (i=0; i<n; i++) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                x = m_<%=name%>(x);
                SET_DATA_INDEX(p2,idx2,dtype,x);
            }
        } else {
            //<% if need_align %>
            if (is_aligned(p1,sizeof(dtype)) &&
                is_aligned(p2,sizeof(dtype)) ) {
                if (s1 == sizeof(dtype) &&
                    s2 == sizeof(dtype) ) {
                    for (i=0; i<n; i++) {
                        ((dtype*)p2)[i] = m_<%=name%>(((dtype*)p1)[i]);
                    }
                    return;
                }
                if (is_aligned_step(s1,sizeof(dtype)) &&
                    is_aligned_step(s2,sizeof(dtype)) ) {
                    //<% end %>
                    for (i=0; i<n; i++) {
                        *(dtype*)p2 = m_<%=name%>(*(dtype*)p1);
                        p1 += s1;
                        p2 += s2;
                    }
                    return;
                    //<% if need_align %>
                }
            }
            for (i=0; i<n; i++) {
                GET_DATA_STRIDE(p1,s1,dtype,x);
                x = m_<%=name%>(x);
                SET_DATA_STRIDE(p2,s2,dtype,x);
            }
            //<% end %>
        }
    }
}

/*
  Unary <%=name%>.
  @overload <%=op_map%>
  @return [Numo::<%=class_name%>] <%=name%> of self.
*/
static VALUE
<%=c_func(0)%>(VALUE self)
{
    ndfunc_arg_in_t ain[1] = {{cT,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = {<%=c_iter%>, FULL_LOOP, 1,1, ain,aout};

    return na_ndloop(&ndf, 1, self);
}
