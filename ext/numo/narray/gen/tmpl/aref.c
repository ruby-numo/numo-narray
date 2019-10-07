/*
  Multi-dimensional element reference.
  @overload [](dim0,...,dimL)
  @param [Numeric,Range,Array,Numo::Int32,Numo::Int64,Numo::Bit,TrueClass,FalseClass,Symbol] dim0,...,dimL  multi-dimensional indices.
  @return [Numeric,Numo::<%=class_name%>] an element or NArray view.
  @see Numo::NArray#[]
  @see #[]=
 */
static VALUE
<%=c_func(-1)%>(int argc, VALUE *argv, VALUE self)
{
    int nd;
    size_t pos;
    char *ptr;

    nd = na_get_result_dimension(self, argc, argv, sizeof(dtype), &pos);
    if (nd) {
        return na_aref_main(argc, argv, self, 0, nd);
    } else {
        ptr = na_get_pointer_for_read(self) + pos;
        return m_extract(ptr);
    }
}
