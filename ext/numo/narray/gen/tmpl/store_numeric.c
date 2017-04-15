static VALUE
<%=c_func(:nodef)%>(VALUE self, VALUE obj)
{
    dtype x;
    x = m_num_to_data(obj);
    obj = <%=type_name%>_new_dim0(x);
    <%=parent.c_func%>(self,obj);
    return self;
}
