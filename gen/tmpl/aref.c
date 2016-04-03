/*
  Array Slicing.
  @overload [](*args)
  @param [Numeric,Range,etc] args  Array of Index for each dimention.
  @return [Numeric,NArray::<%=class_name%>] Element of Array or NArray View.
*/
static VALUE
<%=c_func%>(int argc, VALUE *argv, VALUE self)
{
    ssize_t pos;
    char *ptr;

    pos = na_get_scalar_position(self, argc, argv, sizeof(dtype));
    if (pos == -1) {
        return na_aref_main(argc, argv, self, 0);
    } else {
        ptr = na_get_pointer_for_read(self) + pos;
        return m_extract(ptr);
    }
}
