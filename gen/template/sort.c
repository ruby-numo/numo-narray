static void
<%=c_iterator%>(na_loop_t *const lp)
{
    size_t i, n;
    char *ptr;
    ssize_t step;
    size_t *idx;
    dtype *buf;

    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, ptr, step, idx);
    if (idx) {
        buf = (dtype*)(lp->opt_ptr);
        for (i=0; i<n; i++) {
            buf[i] = *(dtype*)(ptr+idx[i]);
        }
        <%=tp%>_qsort(buf, n, sizeof(dtype));
        for (i=0; i<n; i++) {
            *(dtype*)(ptr+idx[i]) = buf[i];
        }
    } else {
        <%=tp%>_qsort(ptr, n, step);
    }
}

static VALUE
<%=c_instance_method%>(int argc, VALUE *argv, VALUE self)
{
    return na_sort_main(argc, argv, self, <%=c_iterator%>);
}
