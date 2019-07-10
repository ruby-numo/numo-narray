static void
<%=c_iter%>(na_loop_t *const lp)
{
    size_t     i;
    BIT_DIGIT *a1, *a2;
    size_t     p1,  p2;
    ssize_t    s1,  s2;
    size_t    *idx1, *idx2;
    BIT_DIGIT  x=0, y=0;

    INIT_COUNTER(lp, i);
    INIT_PTR_BIT_IDX(lp, 0, a1, p1, s1, idx1);
    INIT_PTR_BIT_IDX(lp, 1, a2, p2, s2, idx2);
    if (idx2) {
        if (idx1) {
            for (; i--;) {
                LOAD_BIT(a2, p2+*idx2, y);
                if (y == <%=init_bit%>) {
                    LOAD_BIT(a1, p1+*idx1, x);
                    if (x != <%=init_bit%>) {
                        STORE_BIT(a2, p2+*idx2, x);
                    }
                }
                idx1++;
                idx2++;
            }
        } else {
            for (; i--;) {
                LOAD_BIT(a2, p2+*idx2, y);
                if (y == <%=init_bit%>) {
                    LOAD_BIT(a1, p1, x);
                    if (x != <%=init_bit%>) {
                        STORE_BIT(a2, p2+*idx2, x);
                    }
                }
                p1 += s1;
                idx2++;
            }
        }
    } else if (s2) {
        if (idx1) {
            for (; i--;) {
                LOAD_BIT(a2, p2, y);
                if (y == <%=init_bit%>) {
                    LOAD_BIT(a1, p1+*idx1, x);
                    if (x != <%=init_bit%>) {
                        STORE_BIT(a2, p2, x);
                    }
                }
                idx1++;
                p2 += s2;
            }
        } else {
            for (; i--;) {
                LOAD_BIT(a2, p2, y);
                if (y == <%=init_bit%>) {
                    LOAD_BIT(a1, p1, x);
                    if (x != <%=init_bit%>) {
                        STORE_BIT(a2, p2, x);
                    }
                }
                p1 += s1;
                p2 += s2;
            }
        }
    } else {
        LOAD_BIT(a2, p2, x);
        if (x != <%=init_bit%>) {
            return;
        }
        if (idx1) {
            for (; i--;) {
                LOAD_BIT(a1, p1+*idx1, y);
                if (y != <%=init_bit%>) {
                    STORE_BIT(a2, p2, y);
                    return;
                }
                idx1++;
            }
        } else {
            for (; i--;) {
                LOAD_BIT(a1, p1, y);
                if (y != <%=init_bit%>) {
                    STORE_BIT(a2, p2, y);
                    return;
                }
                p1 += s1;
            }
        }
    }
}

/*
<% case name
   when /^any/ %>
  Return true if any of bits is one (true).
<% when /^all/ %>
  Return true if all of bits are one (true).
<% end %>
  If argument is supplied, return Bit-array reduced along the axes.
  @overload <%=op_map%>(axis:nil, keepdims:false)
  @param [Integer,Array,Range] axis (keyword) axes to be reduced.
  @param [TrueClass] keepdims (keyword) If true, the reduced axes are left in the result array as dimensions with size one.
  @return [Numo::Bit] .
*/
static VALUE
<%=c_func(-1)%>(int argc, VALUE *argv, VALUE self)
{
    VALUE v, reduce;
    narray_t *na;
    ndfunc_arg_in_t ain[3] = {{cT,0},{sym_reduce,0},{sym_init,0}};
    ndfunc_arg_out_t aout[1] = {{numo_cBit,0}};
    ndfunc_t ndf = {<%=c_iter%>, FULL_LOOP_NIP, 3,1, ain,aout};

    GetNArray(self,na);
    if (NA_SIZE(na)==0) {
        return Qfalse;
    }
    reduce = na_reduce_dimension(argc, argv, 1, &self, &ndf, 0);
    v = na_ndloop(&ndf, 3, self, reduce, INT2FIX(<%=init_bit%>));
    if (argc > 0) {
        return v;
    }
    v = <%=find_tmpl("extract").c_func%>(v);
    switch (v) {
    case INT2FIX(0):
        return Qfalse;
    case INT2FIX(1):
        return Qtrue;
    default:
        rb_bug("unexpected result");
        return v;
    }
}
