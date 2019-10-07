/*
  Multi-dimensional element assignment.
  @overload []=(dim0,...,dimL,val)
  @param [Numeric,Range,Array,Numo::Int32,Numo::Int64,Numo::Bit,TrueClass,FalseClass,Symbol] dim0,...,dimL  multi-dimensional indices.
  @param [Numeric,Numo::NArray,Array] val  Value(s) to be set to self.
  @return [Numeric,Numo::NArray,Array] returns `val` (last argument).
  @see Numo::NArray#[]=
  @see #[]
*/
static VALUE
<%=c_func(-1)%>(int argc, VALUE *argv, VALUE self)
{
    int nd;
    size_t pos;
    char *ptr;
    VALUE a;
    dtype x;

    argc--;
    if (argc==0) {
        <%=c_func.sub(/_aset/,"_store")%>(self, argv[argc]);
    } else {
        nd = na_get_result_dimension(self, argc, argv, sizeof(dtype), &pos);
        if (nd) {
            a = na_aref_main(argc, argv, self, 0, nd);
            <%=c_func.sub(/_aset/,"_store")%>(a, argv[argc]);
        } else {
            x = <%=type_name%>_extract_data(argv[argc]);
            ptr = na_get_pointer_for_read_write(self) + pos;
            *(dtype*)ptr = x;
        }

    }
    return argv[argc];
}
