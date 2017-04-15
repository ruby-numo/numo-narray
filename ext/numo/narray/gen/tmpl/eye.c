static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t   n0, n1;
    size_t   i0, i1;
    ssize_t  s0, s1;
    char    *p0, *p1;
    char    *g;
    ssize_t kofs;
    dtype   data;

    g = (char*)(lp->opt_ptr);
    kofs = *(ssize_t*)g;
    data = *(dtype*)(g+sizeof(ssize_t));

    n0 = lp->args[0].shape[0];
    n1 = lp->args[0].shape[1];
    s0 = lp->args[0].iter[0].step;
    s1 = lp->args[0].iter[1].step;
    p0 = NDL_PTR(lp,0);

    for (i0=0; i0 < n0; i0++) {
        p1 = p0;
        for (i1=0; i1 < n1; i1++) {
            *(dtype*)p1 = (i0+kofs==i1) ? data : m_zero;
            p1 += s1;
        }
        p0 += s0;
    }
}

/*
  Eye: Set a value to diagonal components, set 0 to non-diagonal components.
  @overload <%=name%>([element,offset])
  @param [Numeric] element  Diagonal element to be stored. Default is 1.
  @param [Integer] offset Diagonal offset from the main diagonal.  The
      default is 0. k>0 for diagonals above the main diagonal, and k<0
      for diagonals below the main diagonal.
  @return [Numo::<%=class_name%>] <%=name%> of self.
*/
static VALUE
<%=c_func(-1)%>(int argc, VALUE *argv, VALUE self)
{
    ndfunc_arg_in_t ain[1] = {{OVERWRITE,2}};
    ndfunc_t ndf = {<%=c_iter%>, NO_LOOP, 1,0, ain,0};
    ssize_t kofs;
    dtype data;
    char *g;
    int nd;
    narray_t *na;

    // check arguments
    if (argc > 2) {
        rb_raise(rb_eArgError,"too many arguments (%d for 0..2)",argc);
    } else if (argc == 2) {
        data = m_num_to_data(argv[0]);
        kofs = NUM2SSIZET(argv[1]);
    } else if (argc == 1) {
        data = m_num_to_data(argv[0]);
        kofs = 0;
    } else {
        data = m_one;
        kofs = 0;
    }

    GetNArray(self,na);
    nd = na->ndim;
    if (nd < 2) {
        rb_raise(nary_eDimensionError,"less than 2-d array");
    }

    // Diagonal offset from the main diagonal.
    if (kofs >= 0) {
        if ((size_t)(kofs) >= na->shape[nd-1]) {
            rb_raise(rb_eArgError,"invalid diagonal offset(%"SZF"d) for "
                     "last dimension size(%"SZF"d)",kofs,na->shape[nd-1]);
        }
    } else {
        if ((size_t)(-kofs) >= na->shape[nd-2]) {
            rb_raise(rb_eArgError,"invalid diagonal offset(%"SZF"d) for "
                     "last-1 dimension size(%"SZF"d)",kofs,na->shape[nd-2]);
        }
    }

    g = ALLOCA_N(char,sizeof(ssize_t)+sizeof(dtype));
    *(ssize_t*)g = kofs;
    *(dtype*)(g+sizeof(ssize_t)) = data;

    na_ndloop3(&ndf, g, 1, self);
    return self;
}
