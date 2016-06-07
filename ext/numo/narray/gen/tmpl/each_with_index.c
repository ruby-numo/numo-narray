void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t i, s1;
    char *p1;
    size_t *idx1;
    dtype x;
    VALUE y;
    VALUE a;
    size_t *c;
    int j, nd;

    c = (size_t*)(lp->opt_ptr);
    nd = lp->ndim - 1;

    INIT_COUNTER(lp, i);
    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    c[nd] = 0;
    if (idx1) {
        for (; i--;) {
            a = rb_ary_new2(nd+2);
            GET_DATA_INDEX(p1,idx1,dtype,x);
            y = m_data_to_num(x);
            rb_ary_push(a,y);
            for (j=0; j<=nd; j++) {
                rb_ary_push(a,SIZE2NUM(c[j]));
            }
            rb_yield(a);
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
            rb_yield(a);
            c[nd]++;
        }
    }
}

/*
  Invokes the given block once for each element of self,
  passing that element and indices along each axis as parameters.
  @overload <%=method%>
  @return [Numo::NArray] self
  For a block {|x,i,j,...| ... }
  @yield [x,i,j,...]  x is an element, i,j,... are multidimensional indices.
*/
static VALUE
<%=c_func%>(VALUE self)
{
    ndfunc_arg_in_t ain[1] = {{Qnil,0}};
    ndfunc_t ndf = {<%=c_iter%>, FULL_LOOP_NIP, 1,0, ain,0};

    na_ndloop_with_index(&ndf, 1, self);
    return self;
}
