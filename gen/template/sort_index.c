static VALUE
<%=c_instance_method%>(int argc, VALUE *argv, VALUE self)
{
    return na_sort_index_main(argc, argv, self, <%=tp%>_index_qsort);
}
