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
<%=c_function%>(VALUE self, VALUE obj)
{
    dtype x;
    x = m_num_to_data(obj);
    obj = nary_<%=tp%>_new_dim0(x);
    <%=T["store"].c_instance_method%>(self,obj);
    return self;
}
