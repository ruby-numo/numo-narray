static VALUE
<%=c_iter%>(char *ptr, size_t pos, VALUE fmt)
{
    dtype x;
    LOAD_BIT(ptr,pos,x);
    return format_<%=tp%>(fmt, x);
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
