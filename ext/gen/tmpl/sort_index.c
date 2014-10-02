/*
  Returns index narray of sort.
  @overload sort_index
  @return [NArray::<%=class_name%>] index narray of sort.
*/
static VALUE
<%=c_func%>(int argc, VALUE *argv, VALUE self)
{
    return na_sort_index_main(argc, argv, self, <%=tp%>_index_qsort);
}
