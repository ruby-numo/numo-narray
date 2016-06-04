static void
<%=c_iter%>(na_loop_t *const lp)
{
    //int      nd;
    size_t   n0, n1;
    size_t   i0, i1;
    ssize_t  s0, s1;
    char    *p0, *p1;

    nd = lp->args[0].ndim;
    n0 = lp->args[0].shape[0];
    n1 = lp->args[0].shape[1];
    s0 = lp->args[0].iter[0].step;
    s1 = lp->args[0].iter[1].step;
    p0 = NDL_PTR(lp,0);
    //printf("lp->ndim=%d nd=%d n0=%ld n1=%ld s0=%ld s1=%ld\n",lp->ndim,nd,n0,n1,s0,s1);

    for (i0=0; i0 < n0; i0++) {
        p1 = p0;
        for (i1=0; i1 < n1; i1++) {
            *(dtype*)p1 = (i0==i1) ? m_one : m_zero;
            p1 += s1;
        }
        p0 += s0;
    }
}

/*
  Eye: set 1 to diagonal components, set 0 to non-diagonal components.
  @overload <%=method%>
  @return [Numo::<%=class_name%>] self.
*/
static VALUE
<%=c_func%>(VALUE self, VALUE val)
{
    ndfunc_arg_in_t ain[1] = {{OVERWRITE,2}};
    ndfunc_t ndf = {<%=c_iter%>, NO_LOOP, 1,0, ain,0};

    na_ndloop(&ndf, 1, self);
    return self;
}
