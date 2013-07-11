VALUE
nary_<%=tp%>_extract(VALUE self)
{
    volatile VALUE v;
    char *ptr;
    narray_t *na;
    GetNArray(self,na);

    if (na->ndim==0) {
        ptr = na_get_pointer_for_read(self) + na_get_offset(self);
        v = m_extract(ptr);
        na_release_lock(self);
        return v;
    }
    return self;
}
