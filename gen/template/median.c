static void
<%=c_iterator%>(na_loop_t *const lp)
{
    size_t i, n;
    char *p1, *p2;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    dtype *buf;

    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2, idx2);
    buf = (dtype*)(lp->opt_ptr);
    if (idx1) {
        for (i=0; i<n; i++) {
            buf[i] = *(dtype*)(p1+idx1[i]);
        }
    } else {
        for (i=0; i<n; i++) {
            buf[i] = *(dtype*)(p1+s1*i);
        }
    }
    <%=tp%>_qsort(buf, n, sizeof(dtype));
    for (; n; n--) {
        if (!isnan(buf[n-1])) break;
    }
    if (n==0) {
        *(dtype*)p2 = buf[0];
    }
    else if (n%2==0) {
        *(dtype*)p2 = (buf[n/2-1]+buf[n/2])/2;
    }
    else {
        *(dtype*)p2 = buf[(n-1)/2];
    }
}

static VALUE
<%=c_instance_method%>(int argc, VALUE *argv, VALUE self)
{
    VALUE v;
    v = na_median_main(argc, argv, self, <%=c_iterator%>);
    return nary_<%=tp%>_extract(v);
}
