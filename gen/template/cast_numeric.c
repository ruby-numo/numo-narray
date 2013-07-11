static VALUE
 nary_<%=tp%>_new_dim0(dtype x)
{
    narray_t *na;
    VALUE v;
    dtype *ptr;

    v = rb_narray_new(cT, 0, NULL);
    GetNArray(v,na);
    ptr = (dtype*)(char*)na_get_pointer_for_write(v);
    *ptr = x;
    na_release_lock(v);
    return v;
}

static VALUE
<%=c_function%>(VALUE x)
{
    dtype y;
    y = m_num_to_data(x);
    return nary_<%=tp%>_new_dim0(y);
}
