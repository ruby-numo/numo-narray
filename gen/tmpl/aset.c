/*
  Array Set.
  @overload []=(idx0,..,idxN,val)
  @param [Numeric,Range,etc] idx0,..,idxN  Multi-dimensional Index.
  @param [Numeric,NArray,etc] val  Value(s) to be set to self.
  @return [Numeric] returns val (last argument).
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
