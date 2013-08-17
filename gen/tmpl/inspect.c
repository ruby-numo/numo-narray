static VALUE
<%=c_iter%>(char *ptr, size_t pos, VALUE fmt)
{
    return format_<%=tp%>(fmt, (dtype*)(ptr+pos));
}

/*
  Returns a string containing a human-readable representation of NArray.
  @overload inspect
  @return [String]
*/
VALUE
<%=c_func%>(VALUE ary)
{
    VALUE str = na_info_str(ary);
    na_ndloop_inspect(ary, str, <%=c_iter%>, Qnil);
    return str;
}
