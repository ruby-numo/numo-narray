static VALUE
<%=c_func%>(VALUE self, VALUE obj)
{
    return <%=find_tmpl("store").c_func%>(self,<%=find_tmpl("cast_array").c_func%>(obj));
}
