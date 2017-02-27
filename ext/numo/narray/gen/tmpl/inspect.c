static VALUE
<%=c_iter%>(char *ptr, size_t pos, VALUE fmt)
{
<% if is_object %>
    return rb_inspect(*(VALUE*)(ptr+pos));
<% else %>
    return format_<%=tp%>(fmt, (dtype*)(ptr+pos));
<% end %>
}

/*
  Returns a string containing a human-readable representation of NArray.
  @overload inspect
  @return [String]
*/
VALUE
<%=c_func%>(VALUE ary)
{
    return na_ndloop_inspect(ary, <%=c_iter%>, Qnil);
}
