/*
  Array Slicing.
  @overload [](*args)
  @param [Numeric,Range,etc] args  Array of Index for each dimention.
  @return [Numeric,NArray::<%=class_name%>] Element of Array or NArray View.
*/
static VALUE
<%=c_func%>(int argc, VALUE *argv, VALUE self)
{
    int i;
    size_t pos;
    ssize_t x, stride, *idx;
    char *ptr;
    narray_t *na;

    // scalar check: element extraction
    GetNArray(self,na);
    if (argc==1) {
        switch(TYPE(argv[0])) {
        case T_FIXNUM:
            x = FIX2LONG(argv[0]);
            break;
        case T_BIGNUM:
        case T_FLOAT:
            x = NUM2SSIZE(argv[0]);
            break;
        default:
            goto non_scalar;
        }
        stride = na_dtype_elmsz(cT);
        pos = na_single_dim_scalar_position(self, x, stride);
    } else
    if (argc==na->ndim) {
        idx = ALLOCA_N(ssize_t, argc);
        for (i=0; i<argc; i++) {
            switch(TYPE(argv[i])) {
            case T_FIXNUM:
                idx[i] = FIX2LONG(argv[i]);
                break;
            case T_BIGNUM:
            case T_FLOAT:
                idx[i] = NUM2SSIZE(argv[i]);
                break;
            default:
                goto non_scalar;
            }
        }
        stride = na_dtype_elmsz(cT);
        pos = na_multi_dim_scalar_position(self, argc, idx, stride);
    } else {
        goto non_scalar;
    }
    ptr = na_get_pointer_for_read(self) + pos;
    return m_extract(ptr);

 non_scalar:
    return na_aref_main(argc, argv, self, 0);
}
