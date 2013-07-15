static void
<%=c_iterator%>(na_loop_t *const lp)
{
    size_t  i;
    char   *p1;
    ssize_t s1;
    size_t *idx1;
    double  x, beg, step;
    dtype   y;
    size_t  c;
    seq_opt_t *g;

    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1, idx1);
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
  @return [NArray::<%=class_name%>] self.
*/
static VALUE
<%=c_instance_method%>(int argc, VALUE *args, VALUE self)
{
    seq_opt_t *g;
    VALUE vbeg=Qnil, vstep=Qnil;
    ndfunc_t *func;

    rb_scan_args(argc, args, "02", &vbeg, &vstep);
    func = ndfunc_alloc(<%=c_iterator%>, FULL_LOOP,
                        1, 0, Qnil);
    g = ALLOCA_N(seq_opt_t,1);
    g->beg = 0;
    g->step = 1;
    g->count = 0;
    if (vbeg!=Qnil) {g->beg = NUM2DBL(vbeg);}
    if (vstep!=Qnil) {g->step = NUM2DBL(vstep);}
    ndloop_do3(func, g, 1, self);
    ndfunc_free(func);
    return self;
}
