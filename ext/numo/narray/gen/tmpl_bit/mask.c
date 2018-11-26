static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t  i;
    BIT_DIGIT *a;
    size_t  p1, p2;
    ssize_t s1, s2;
    size_t *idx1, *idx2, *pidx;
    BIT_DIGIT x=0;
    size_t  count;
    where_opt_t *g;

    g = (where_opt_t*)(lp->opt_ptr);
    count = g->count;
    pidx  = (size_t*)(g->idx1);
    INIT_COUNTER(lp, i);
    INIT_PTR_BIT_IDX(lp, 0, a, p1, s1, idx1);
    //INIT_PTR_IDX(lp, 1, p2, s2, idx2);
    p2 = lp->args[1].iter[0].pos;
    s2 = lp->args[1].iter[0].step;
    idx2 = lp->args[1].iter[0].idx;

    if (idx1) {
        if (idx2) {
            for (; i--;) {
                LOAD_BIT(a, p1+*idx1, x);
                idx1++;
                if (x) {
                    *(pidx++) = p2+*idx2;
                    count++;
                }
                idx2++;
            }
        } else {
            for (; i--;) {
                LOAD_BIT(a, p1+*idx1, x);
                idx1++;
                if (x) {
                    *(pidx++) = p2;
                    count++;
                }
                p2 += s2;
            }
        }
    } else {
        if (idx2) {
            for (; i--;) {
                LOAD_BIT(a, p1, x);
                p1 += s1;
                if (x) {
                    *(pidx++) = p2+*idx2;
                    count++;
                }
                idx2++;
            }
        } else {
            for (; i--;) {
                LOAD_BIT(a, p1, x);
                p1 += s1;
                if (x) {
                    *(pidx++) = p2;
                    count++;
                }
                p2 += s2;
            }
        }
    }
    g->count = count;
    g->idx1  = (char*)pidx;
}

#if   SIZEOF_VOIDP == 8
#define cIndex numo_cInt64
#elif SIZEOF_VOIDP == 4
#define cIndex numo_cInt32
#endif

static void shape_error() {
    rb_raise(nary_eShapeError,"mask and masked arrays must have the same shape");
}

/*
  Return subarray of argument masked with self bit array.
  @overload <%=op_map%>(array)
  @param [Numo::NArray] array  narray to be masked.
  @return [Numo::NArray]  view of masked array.
*/
static VALUE
<%=c_func(1)%>(VALUE mask, VALUE val)
{
    int i;
    VALUE idx_1, view;
    narray_data_t *nidx;
    narray_view_t *nv, *nv_val;
    narray_t      *na, *na_mask;
    stridx_t stridx0;
    size_t n_1;
    where_opt_t g;
    ndfunc_arg_in_t ain[2] = {{cT,0},{Qnil,0}};
    ndfunc_t ndf = {<%=c_iter%>, FULL_LOOP, 2, 0, ain, 0};

    // cast val to NArray
    if (!rb_obj_is_kind_of(val, numo_cNArray)) {
        val = rb_funcall(numo_cNArray, id_cast, 1, val);
    }
    // shapes of mask and val must be same
    GetNArray(val, na);
    GetNArray(mask, na_mask);
    if (na_mask->ndim != na->ndim) {
        shape_error();
    }
    for (i=0; i<na->ndim; i++) {
        if (na_mask->shape[i] != na->shape[i]) {
            shape_error();
        }
    }

    n_1 = NUM2SIZET(<%=find_tmpl("count_true").c_func%>(0, NULL, mask));
    idx_1 = nary_new(cIndex, 1, &n_1);
    g.count = 0;
    g.elmsz = SIZEOF_VOIDP;
    g.idx1 = na_get_pointer_for_write(idx_1);
    g.idx0 = NULL;
    na_ndloop3(&ndf, &g, 2, mask, val);

    view = na_s_allocate_view(rb_obj_class(val));
    GetNArrayView(view, nv);
    na_setup_shape((narray_t*)nv, 1, &n_1);

    GetNArrayData(idx_1,nidx);
    SDX_SET_INDEX(stridx0,(size_t*)nidx->ptr);
    nidx->ptr = NULL;
    RB_GC_GUARD(idx_1);

    nv->stridx = ALLOC_N(stridx_t,1);
    nv->stridx[0] = stridx0;
    nv->offset = 0;

    switch(NA_TYPE(na)) {
    case NARRAY_DATA_T:
        nv->data = val;
        break;
    case NARRAY_VIEW_T:
        GetNArrayView(val, nv_val);
        nv->data = nv_val->data;
        break;
    default:
        rb_raise(rb_eRuntimeError,"invalid NA_TYPE: %d",NA_TYPE(na));
    }

    return view;
}
