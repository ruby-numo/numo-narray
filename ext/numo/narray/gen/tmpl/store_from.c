static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t  i, s1, s2;
    char   *p1, *p2;
    size_t *idx1, *idx2;
    <%=dtype%> x;
    dtype y;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);
    if (idx2) {
        if (idx1) {
            for (; i--;) {
                GET_DATA_INDEX(p2,idx2,<%=dtype%>,x);
                y = <%=macro%>(x);
                SET_DATA_INDEX(p1,idx1,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_INDEX(p2,idx2,<%=dtype%>,x);
                y = <%=macro%>(x);
                SET_DATA_STRIDE(p1,s1,dtype,y);
            }
        }
    } else {
        if (idx1) {
            for (; i--;) {
                GET_DATA_STRIDE(p2,s2,<%=dtype%>,x);
                y = <%=macro%>(x);
                SET_DATA_INDEX(p1,idx1,dtype,y);
            }
        } else {
            for (; i--;) {
                GET_DATA_STRIDE(p2,s2,<%=dtype%>,x);
                y = <%=macro%>(x);
                SET_DATA_STRIDE(p1,s1,dtype,y);
            }
        }
    }
}


static VALUE
<%=c_func(:nodef)%>(VALUE self, VALUE obj)
{
    ndfunc_arg_in_t ain[2] = {{OVERWRITE,0},{Qnil,0}};
    ndfunc_t ndf = { <%=c_iter%>, FULL_LOOP, 2, 0, ain, 0 };

    na_ndloop(&ndf, 2, self, obj);
    return self;
}
