static VALUE
<%=c_function%>(VALUE self, VALUE obj)
{
    return <%=T["store"].c_instance_method%>(self,<%=T["cast_array"].c_function%>(obj));
}
