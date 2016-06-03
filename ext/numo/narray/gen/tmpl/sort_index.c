
<% [64,32].each do |i| %>
#define idx_t int<%=i%>_t
static void
<%=tp%>_index<%=i%>_qsort(na_loop_t *const lp)
{
    size_t   i, n, idx;
    char    *d_ptr, *i_ptr, *o_ptr;
    ssize_t  d_step, i_step, o_step;
    char   **ptr;

    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, d_ptr, d_step);
    INIT_PTR(lp, 1, i_ptr, i_step);
    INIT_PTR(lp, 2, o_ptr, o_step);

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
        idx = (ptr[i] - d_ptr) / d_step;
        //printf("(%ld,%ld)",i,idx);
        *(idx_t*)o_ptr = *(idx_t*)(i_ptr + i_step * idx);
        o_ptr += o_step;
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
    VALUE idx, tmp, reduce, res;
    char *buf;
    ndfunc_arg_in_t ain[3] = {{Qnil,0},{Qnil,0},{sym_reduce,0}};
    ndfunc_arg_out_t aout[1] = {{0,0,0}};
    ndfunc_t ndf = {0, STRIDE_LOOP_NIP|NDF_FLAT_REDUCE|NDF_CUM, 3,1, ain,aout};

    GetNArray(self,na);
    if (na->ndim==0) {
        return INT2FIX(0);
    }
    if (na->size > (~(u_int32_t)0)) {
        aout[0].type = numo_cInt64;
        idx = rb_narray_new(numo_cInt64, na->ndim, na->shape);
        ndf.func = <%=tp%>_index64_qsort;
    } else {
        aout[0].type = numo_cInt32;
        idx = rb_narray_new(numo_cInt32, na->ndim, na->shape);
        ndf.func = <%=tp%>_index32_qsort;
    }
    rb_funcall(idx, rb_intern("seq"), 0);

    reduce = na_reduce_dimension(argc, argv, 1, &self); // v[0] = self

    size = na->size*sizeof(void*);
    buf = rb_alloc_tmp_buffer(&tmp, size);
    res = na_ndloop3(&ndf, buf, 3, self, idx, reduce);
    rb_free_tmp_buffer(&tmp);
    return res;
}
