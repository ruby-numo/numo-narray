static VALUE
numo_<%=tp%>_new_dim0(dtype x)
{
    VALUE v;
    dtype *ptr;

    v = rb_narray_new(cT, 0, NULL);
    ptr = (dtype*)(char*)na_get_pointer_for_write(v);
    *ptr = x;
    na_release_lock(v);
    return v;
}

static VALUE
<%=c_func%>(VALUE self, VALUE obj)
{
    dtype x;
    x = m_num_to_data(obj);
    obj = numo_<%=tp%>_new_dim0(x);
    <%=find_tmpl("store").c_func%>(self,obj);
    return self;
}
