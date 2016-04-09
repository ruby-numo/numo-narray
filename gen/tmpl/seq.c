static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t  i;
    char   *p1;
    ssize_t s1;
    size_t *idx1;
    double  x, beg, step, c;
    dtype   y;
    seq_opt_t *g;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    g = (seq_opt_t*)(lp->opt_ptr);
    beg  = g->beg;
    step = g->step;
    c    = g->count;
    if (idx1) {
        for (; i--;) {
            x = beg + step * c++;
            y = m_from_double(x);
            *(dtype*)(p1+*idx1) = y;
            idx1++;
        }
    } else {
        for (; i--;) {
            x = beg + step * c++;
            y = m_from_double(x);
            *(dtype*)(p1) = y;
            p1 += s1;
        }
    }
    g->count = c;
}

/*
  Set Sequence of numbers to self NArray.
  @overload seq([beg,[step]])
  @param [Numeric] beg  begining of sequence. (default=0)
  @param [Numeric] step  step of sequence. (default=1)
  @return [Numo::<%=class_name%>] self.
*/
static VALUE
<%=c_func%>(int argc, VALUE *args, VALUE self)
{
    seq_opt_t *g;
    VALUE vbeg=Qnil, vstep=Qnil;
    ndfunc_arg_in_t ain[1] = {{OVERWRITE,0}};
    ndfunc_t ndf = { <%=c_iter%>, FULL_LOOP, 1, 0, ain, 0 };

    g = ALLOCA_N(seq_opt_t,1);
    g->beg = 0;
    g->step = 1;
    g->count = 0;
    rb_scan_args(argc, args, "02", &vbeg, &vstep);
    if (vbeg!=Qnil) {g->beg = NUM2DBL(vbeg);}
    if (vstep!=Qnil) {g->step = NUM2DBL(vstep);}

    na_ndloop3(&ndf, g, 1, self);
    return self;
}
