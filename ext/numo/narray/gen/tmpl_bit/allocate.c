static VALUE
<%=c_func(0)%>(VALUE self)
{
    narray_t *na;
    char *ptr;

    GetNArray(self,na);

    switch(NA_TYPE(na)) {
    case NARRAY_DATA_T:
        ptr = NA_DATA_PTR(na);
        if (na->size > 0 && ptr == NULL) {
            ptr = xmalloc(((na->size-1)/8/sizeof(BIT_DIGIT)+1)*sizeof(BIT_DIGIT));
            NA_DATA_PTR(na) = ptr;
            NA_DATA_OWNED(na) = TRUE;
        }
        break;
    case NARRAY_VIEW_T:
        rb_funcall(NA_VIEW_DATA(na), rb_intern("allocate"), 0);
        break;
    default:
        rb_raise(rb_eRuntimeError,"invalid narray type");
    }
    return self;
}
