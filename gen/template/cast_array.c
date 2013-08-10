static void
<%=c_iterator%>(na_loop_t *const lp)
{
    size_t i, n;
    size_t i1, n1;
    VALUE  v1, *ptr;
    char   *p2;
    size_t s2, *idx2;
    VALUE  x;
    double y;
    dtype  z;
    size_t len, c;
    double beg, step;

    INIT_COUNTER(lp, n);
    v1 = lp->args[0].value;
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);

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
        ptr = &v1;
    }
    for (i=i1=0; i1<n1 && i<n; i++,i1++) {
        x = ptr[i1];
        if (rb_obj_is_kind_of(x, rb_cRange) || rb_obj_is_kind_of(x, na_cStep)) {
            nary_step_sequence(x,&len,&beg,&step);
            for (c=0; c<len && i<n; c++,i++) {
                y = beg + step * c;
                z = m_from_double(y);
                STORE_DATA_STEP(p2, s2, idx2, dtype, z);
            }
        }
        else if (TYPE(x) != T_ARRAY) {
            if (x == Qnil) x = INT2FIX(0);
            z = m_num_to_data(x);
            STORE_DATA_STEP(p2, s2, idx2, dtype, z);
        }
    }
    z = m_zero;
    for (; i<n; i++) {
        STORE_DATA_STEP(p2, s2, idx2, dtype, z);
    }
}

static VALUE
<%=c_function%>(VALUE rary)
{
    int nd;
    size_t *shape;
    VALUE tp, nary;
    narray_t *na;
    ndfunc_arg_in_t ain[2] = {{rb_cArray,0},{Qnil,0}};
    ndfunc_t ndf = { <%=c_iterator%>, FULL_LOOP, 2, 0, ain, 0 };

    shape = na_mdarray_investigate(rary, &nd, &tp);
    nary = rb_narray_new(cT, nd, shape);
    xfree(shape);
    GetNArray(nary,na);
    if (na->size>0) {
        na_alloc_data(nary);
        na_ndloop_cast_rarray_to_narray(&ndf, rary, nary);
    }
    return nary;
}
