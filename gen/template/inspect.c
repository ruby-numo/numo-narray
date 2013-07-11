static VALUE
<%=c_iterator%>(char *ptr, size_t pos, VALUE fmt)
{
    return format_<%=tp%>(fmt, ptr+pos);
}

VALUE
<%=c_instance_method%>(VALUE ary)
{
    VALUE str = na_info_str(ary);
    ndloop_do_inspect(ary, str, <%=c_iterator%>, Qnil);
    return str;
}
