/*
  Array element(s) assignment.
  @overload []=(dim0,..,dimL,val)
  @param [Numeric,Range,Array,Numo::Bit,Numo::Int32,Numo::Int64] dim0,..,dimL  Multi-dimensional Index.
  @param [Numeric,Numo::NArray,etc] val  Value(s) to be set to self.
  @return [Numeric] returns `val` (last argument).

  Replaces element(s) at `dim0`, `dim1`, ... . Broadcasting mechanism is applied.

  @see #[]

  @example
      a = Numo::Bit.new(4,5).fill(0)
      # => Numo::Bit#shape=[4,5]
      # [[0, 0, 0, 0, 0],
      #  [0, 0, 0, 0, 0],
      #  [0, 0, 0, 0, 0],
      #  [0, 0, 0, 0, 0]]

      a[(0..-1)%2,(1..-1)%2] = 1
      a
      # => Numo::Bit#shape=[4,5]
      # [[0, 1, 0, 1, 0],
      #  [0, 0, 0, 0, 0],
      #  [0, 1, 0, 1, 0],
      #  [0, 0, 0, 0, 0]]
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
        nd = na_get_result_dimension(self, argc, argv, 1, &pos);
        if (nd) {
            a = na_aref_main(argc, argv, self, 0, nd);
            <%=c_func.sub(/_aset/,"_store")%>(a, argv[argc]);
        } else {
            x = <%=type_name%>_extract_data(argv[argc]);
            ptr = na_get_pointer_for_read_write(self);
            STORE_BIT(ptr,pos,x);
        }

    }
    return argv[argc];
}
