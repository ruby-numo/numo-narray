static VALUE
<%=c_iter%>(char *ptr, size_t pos, VALUE fmt)
{
    dtype x;
    LOAD_BIT(ptr,pos,x);
    return format_<%=type_name%>(fmt, x);
}

/*
  Returns a string containing a human-readable representation of NArray.
  @overload inspect
  @return [String]
*/
static VALUE
<%=c_func(0)%>(VALUE ary)
{
    return na_ndloop_inspect(ary, <%=c_iter%>, Qnil);
}
