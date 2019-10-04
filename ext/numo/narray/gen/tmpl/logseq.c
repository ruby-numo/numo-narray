typedef struct {
    seq_data_t beg;
    seq_data_t step;
    seq_data_t base;
    seq_count_t count;
} logseq_opt_t;

static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t  i;
    char   *p1;
    ssize_t s1;
    size_t *idx1;
    dtype   x;
    seq_data_t beg, step, base;
    seq_count_t c;
    logseq_opt_t *g;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    g = (logseq_opt_t*)(lp->opt_ptr);
    beg  = g->beg;
    step = g->step;
    base = g->base;
    c    = g->count;
    if (idx1) {
        for (; i--;) {
            x = f_seq(beg,step,c++);
            *(dtype*)(p1+*idx1) = m_pow(base,x);
            idx1++;
        }
    } else {
        for (; i--;) {
            x = f_seq(beg,step,c++);
            *(dtype*)(p1) = m_pow(base,x);
            p1 += s1;
        }
    }
    g->count = c;
}

/*
  Set logarithmic sequence of numbers to self. The sequence is obtained from
     `base**(beg+i*step)`
  where i is 1-dimensional index.
  Applicable classes: DFloat, SFloat, DComplex, SCopmplex.

  @overload logseq(beg,step,[base])
  @param [Numeric] beg  The begining of sequence.
  @param [Numeric] step  The step of sequence.
  @param [Numeric] base  The base of log space. (default=10)
  @return [Numo::<%=class_name%>] self.

  @example
    Numo::DFloat.new(5).logseq(4,-1,2)
    # => Numo::DFloat#shape=[5]
    # [16, 8, 4, 2, 1]

    Numo::DComplex.new(5).logseq(0,1i*Math::PI/3,Math::E)
    # => Numo::DComplex#shape=[5]
    # [1+7.26156e-310i, 0.5+0.866025i, -0.5+0.866025i, -1+1.22465e-16i, ...]
*/
static VALUE
<%=c_func(-1)%>(int argc, VALUE *args, VALUE self)
{
    logseq_opt_t *g;
    VALUE vbeg, vstep, vbase;
    ndfunc_arg_in_t ain[1] = {{OVERWRITE,0}};
    ndfunc_t ndf = {<%=c_iter%>, FULL_LOOP, 1,0, ain,0};

    g = ALLOCA_N(logseq_opt_t,1);
    rb_scan_args(argc, args, "21", &vbeg, &vstep, &vbase);
    g->beg = m_num_to_data(vbeg);
    g->step = m_num_to_data(vstep);
    if (vbase==Qnil) {
        g->base = m_from_real(10);
    } else {
        g->base = m_num_to_data(vbase);
    }
    na_ndloop3(&ndf, g, 1, self);
    return self;
}
