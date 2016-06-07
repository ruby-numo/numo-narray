void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t  i;
    char   *p1, *p2;
    ssize_t s1, s2;
    size_t *idx1, *idx2;
    dtype x;
    VALUE y;
    VALUE a;
    size_t *c;
    int j, nd;

    c = (size_t*)(lp->opt_ptr);
    nd = lp->ndim - 1;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR_IDX(lp, 1, p2, s2, idx2);

    c[nd] = 0;
    if (idx1) {
        if (idx2) {
            for (; i--;) {
                a = rb_ary_new2(nd+2);
                GET_DATA_INDEX(p1,idx1,dtype,x);
                y = m_data_to_num(x);
                rb_ary_push(a,y);
                for (j=0; j<=nd; j++) {
                    rb_ary_push(a,SIZE2NUM(c[j]));
                }
                y = rb_yield(a);
                x = m_num_to_data(y);
                SET_DATA_INDEX(p2,idx2,dtype,x);
                c[nd]++;
            }
        } else {
            for (; i--;) {
                a = rb_ary_new2(nd+2);
                GET_DATA_INDEX(p1,idx1,dtype,x);
                y = m_data_to_num(x);
                rb_ary_push(a,y);
                for (j=0; j<=nd; j++) {
                    rb_ary_push(a,SIZE2NUM(c[j]));
                }
                y = rb_yield(a);
                x = m_num_to_data(y);
                SET_DATA_STRIDE(p2,s2,dtype,x);
                c[nd]++;
            }
        }
    } else {
        if (idx2) {
            for (; i--;) {
                a = rb_ary_new2(nd+2);
                GET_DATA_STRIDE(p1,s1,dtype,x);
                y = m_data_to_num(x);
                rb_ary_push(a,y);
                for (j=0; j<=nd; j++) {
                    rb_ary_push(a,SIZE2NUM(c[j]));
                }
                y = rb_yield(a);
                x = m_num_to_data(y);
                SET_DATA_INDEX(p2,idx2,dtype,x);
                c[nd]++;
            }
        } else {
            for (; i--;) {
                a = rb_ary_new2(nd+2);
                GET_DATA_STRIDE(p1,s1,dtype,x);
                y = m_data_to_num(x);
                rb_ary_push(a,y);
                for (j=0; j<=nd; j++) {
                    rb_ary_push(a,SIZE2NUM(c[j]));
                }
                y = rb_yield(a);
                x = m_num_to_data(y);
                SET_DATA_STRIDE(p2,s2,dtype,x);
                c[nd]++;
            }
        }
    }
}

/*
  Invokes the given block once for each element of self,
  passing that element and indices along each axis as parameters.
  Creates a new NArray containing the values returned by the block.
  Inplace option is allowed, i.e., `nary.inplace.map` overwrites `nary`.

  @overload <%=method%>

  For a block {|x,i,j,...| ... }
  @yield [x,i,j,...]  x is an element, i,j,... are multidimensional indices.

  @return [Numo::NArray] mapped array

*/
static VALUE
<%=c_func%>(VALUE self)
{
    ndfunc_arg_in_t ain[1] = {{Qnil,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = {<%=c_iter%>, FULL_LOOP, 1,1, ain,aout};

    return na_ndloop_with_index(&ndf, 1, self);
}
