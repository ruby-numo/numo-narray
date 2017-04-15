static void
na_gc_mark_robj(narray_data_t* na)
{
    size_t n, i;
    VALUE *a;

    if (na->ptr) {
        a = (VALUE*)(na->ptr);
        n = na->base.size;
        for (i=0; i<n; i++) {
            rb_gc_mark(a[i]);
        }
    }
}

void na_free(narray_data_t* na);

VALUE
<%=c_func(0)%>(VALUE klass)
{
    narray_data_t *na = ALLOC(narray_data_t);

    na->base.ndim = 0;
    na->base.type = NARRAY_DATA_T;
    na->base.flag[0] = 0;
    na->base.flag[1] = 0;
    na->base.size = 0;
    na->base.shape = NULL;
    na->base.reduce = INT2FIX(0);
    na->ptr = NULL;
    return Data_Wrap_Struct(klass, na_gc_mark_robj, na_free, na);
}
