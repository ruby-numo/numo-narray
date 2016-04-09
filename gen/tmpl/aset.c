/*
  Array element(s) set.
  @overload []=(dim0,..,dimL,val)
  @param [Numeric,Range,etc] dim0,..,dimL  Multi-dimensional Index.
  @param [Numeric,Numo::NArray,etc] val  Value(s) to be set to self.
  @return [Numeric] returns val (last argument).

  --- Replace element(s) at +dim0+, +dim1+, ... (index/range/array/true
  for each dimention). Broadcasting mechanism is applied.

      a = Numo::DFloat.new(3,4).seq
      => Numo::DFloat#shape=[3,4]
      [[0, 1, 2, 3],
       [4, 5, 6, 7],
       [8, 9, 10, 11]]

      a[1,2]=99
      a
      => Numo::DFloat#shape=[3,4]
      [[0, 1, 2, 3],
       [4, 5, 99, 7],
       [8, 9, 10, 11]]

      a[1,[0,2]] = [101,102]
      a
      => Numo::DFloat#shape=[3,4]
      [[0, 1, 2, 3],
       [101, 5, 102, 7],
       [8, 9, 10, 11]]

      a[1,true]=99
      a
      => Numo::DFloat#shape=[3,4]
      [[0, 1, 2, 3],
       [99, 99, 99, 99],
       [8, 9, 10, 11]]

*/
static VALUE
<%=c_func%>(int argc, VALUE *argv, VALUE self)
{
    ssize_t pos;
    char *ptr;
    VALUE a;

    argc--;
    if (argc==0) {
        <%=c_func.sub(/_aset/,"_store")%>(self, argv[argc]);
    } else {
        pos = na_get_scalar_position(self, argc, argv, sizeof(dtype));
        if (pos == -1) {
            a = na_aref_main(argc, argv, self, 0);
            <%=c_func.sub(/_aset/,"_store")%>(a, argv[argc]);
        } else {
            ptr = na_get_pointer_for_read(self) + pos;
            *(dtype*)ptr = m_num_to_data(argv[argc]);
        }

    }
    return argv[argc];
}
