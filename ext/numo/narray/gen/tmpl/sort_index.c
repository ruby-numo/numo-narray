
<% [64,32].each do |i| %>
#define idx_t int<%=i%>_t
static void
<%=tp%>_index<%=i%>_qsort(na_loop_t *const lp)
{
    size_t   i, n;
    char    *d_ptr, *i_ptr;
    ssize_t  d_step, i_step;
    char   **ptr;
    idx_t    idx;

    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, d_ptr, d_step);
    INIT_PTR(lp, 1, i_ptr, i_step);

    ptr = (char**)(lp->opt_ptr);
    //printf("(d_ptr=%lx,ptr=%lx,i_ptr=%lx)\n",d_ptr,ptr,i_ptr);
    for (i=0; i<n; i++) {
        ptr[i] = d_ptr + d_step * i;
        //printf("(%ld,%.3f)",i,*(double*)ptr[i]);
    }

    <%=tp%>_index_qsort(ptr, n, sizeof(dtype*));

    //printf("n=%ld d_step=%ld i_step=%ld\n",n,d_step,i_step);

    d_ptr = lp->args[0].ptr;
    //printf("(d_ptr=%lx,ptr=%lx,i_ptr=%lx)\n",d_ptr,ptr,i_ptr);
    for (i=0; i<n; i++) {
        idx = (idx_t)((ptr[i] - d_ptr) / d_step);
        //printf("(%d,%d,%lx)",i,idx,i_ptr);
        *(idx_t*)i_ptr = idx;
        i_ptr += i_step;
    }
}
#undef idx_t
<% end %>

/*
 *  call-seq:
 *     narray.sort_index() => narray
 *     narray.sort_index(dim0,dim1,..) => narray
 *
 *  Return an index array of sort result.
 *
 *     Numo::NArray[3,4,1,2].sort_index => Numo::Int32[2,3,0,1]
 */
static VALUE
<%=c_func%>(int argc, VALUE *argv, VALUE self)
{
    size_t size;
    narray_t *na;
    VALUE idx, tmp, reduce;
    char *buf;
    ndfunc_arg_in_t ain[3] = {{Qnil,0},{Qnil,0},{sym_reduce,0}};
    ndfunc_t ndf = {0, STRIDE_LOOP_NIP|NDF_FLAT_REDUCE, 3,0, ain,0};

    GetNArray(self,na);
    if (na->ndim==0) {
        return INT2FIX(0);
    }
    if (na->size > (~(u_int32_t)0)) {
        idx = rb_narray_new(numo_cInt64, na->ndim, na->shape);
        ndf.func = <%=tp%>_index64_qsort;
    } else {
        idx = rb_narray_new(numo_cInt32, na->ndim, na->shape);
        ndf.func = <%=tp%>_index32_qsort;
    }
    rb_funcall(idx, rb_intern("allocate"), 0);

    reduce = na_reduce_dimension(argc, argv, 1, &self); // v[0] = self

    size = na->size*sizeof(void*);
    buf = rb_alloc_tmp_buffer(&tmp, size);
    na_ndloop3(&ndf, buf, 3, self, idx, reduce);
    rb_free_tmp_buffer(&tmp);
    return idx;
}
