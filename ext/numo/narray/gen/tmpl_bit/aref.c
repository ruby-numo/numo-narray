/*
  Multi-dimensional element reference.
  @overload [](dim0,...,dimL)
  @param [Numeric,Range,Array,Numo::Int32,Numo::Int64,Numo::Bit,TrueClass,FalseClass,Symbol] dim0,...,dimL  multi-dimensional indices.
  @return [Numeric,Numo::Bit] an element or NArray view.
  @see Numo::NArray#[]
  @see #[]=

  @example
      a = Numo::Int32.new(3,4).seq
      # => Numo::Int32#shape=[3,4]
      # [[0, 1, 2, 3],
      #  [4, 5, 6, 7],
      #  [8, 9, 10, 11]]

      b = (a%2).eq(0)
      # => Numo::Bit#shape=[3,4]
      # [[1, 0, 1, 0],
      #  [1, 0, 1, 0],
      #  [1, 0, 1, 0]]

      b[true,(0..-1)%2]
      # => Numo::Bit(view)#shape=[3,2]
      # [[1, 1],
      #  [1, 1],
      #  [1, 1]]

      b[1,1]
      # => 0
 */
static VALUE
<%=c_func(-1)%>(int argc, VALUE *argv, VALUE self)
{
    int nd;
    size_t pos;
    char *ptr;
    dtype x;

    nd = na_get_result_dimension(self, argc, argv, 1, &pos);
    if (nd) {
        return na_aref_main(argc, argv, self, 0, nd);
    } else {
        ptr = na_get_pointer_for_read(self);
        LOAD_BIT(ptr,pos,x);
        return m_data_to_num(x);
    }
}
