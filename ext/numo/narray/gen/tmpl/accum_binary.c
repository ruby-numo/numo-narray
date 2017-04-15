<% (is_float ? ["","_nan"] : [""]).each do |j| %>
static void
<%=c_iter%><%=j%>(na_loop_t *const lp)
{
    size_t   i;
    char    *p1, *p2, *p3;
    ssize_t  s1, s2, s3;
    dtype    x, y, z;

    INIT_COUNTER(lp, i);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR(lp, 2, p3, s3);
    if (s3==0) {
        // Reduce loop
        GET_DATA(p3,dtype,z);
        for (; i--;) {
            GET_DATA_STRIDE(p1,s1,dtype,x);
            GET_DATA_STRIDE(p2,s2,dtype,y);
            m_<%=name%><%=j%>(x,y,z);
        }
        SET_DATA(p3,dtype,z);
    } else {
        for (; i--;) {
            GET_DATA_STRIDE(p1,s1,dtype,x);
            GET_DATA_STRIDE(p2,s2,dtype,y);
            GET_DATA(p3,dtype,z);
            m_<%=name%><%=j%>(x,y,z);
            SET_DATA_STRIDE(p3,s3,dtype,z);
        }
    }
}
<% end %>

static VALUE
<%=c_func%>_self(int argc, VALUE *argv, VALUE self)
{
    int ignore_nan = 0;
    VALUE v, reduce;
    VALUE naryv[2];
    ndfunc_arg_in_t ain[4] = {{cT,0},{cT,0},{sym_reduce,0},{sym_init,0}};
    ndfunc_arg_out_t aout[1] = {{cT,0}};
    ndfunc_t ndf = { <%=c_iter%>, STRIDE_LOOP_NIP, 4, 1, ain, aout };

    if (argc < 1) {
        rb_raise(rb_eArgError,"wrong number of arguments (%d for >=1)",argc);
    }
    // should fix below: [self.ndim,other.ndim].max or?
    naryv[0] = self;
    naryv[1] = argv[0];
    reduce = na_reduce_dimension(argc-1, argv+1, 2, naryv, &ignore_nan);
<% if is_float %>
    if (ignore_nan) {
        ndf.func = <%=c_iter%>_nan;
    }
<% end %>
    v =  na_ndloop(&ndf, 4, self, argv[0], reduce, m_<%=name%>_init);
    return <%=type_name%>_extract(v);
}

/*
  Binary <%=name%>.

<% if is_float %>
  @overload <%=op_map%>(other, axis:nil, nan:false)
  @param [TrueClass] nan  If true, propagete NaN. If false, ignore NaN.
<% else %>
  @overload <%=op_map%>(other, axis:nil)
<% end %>
  @param [Numo::NArray,Numeric] other
  @param [Numeric,Array,Range] axis  Affected dimensions.
<% if is_float %>
  @param [TrueClass] nan  If true, propagete NaN. If false, ignore NaN.
<% end %>
  @return [Numo::NArray] <%=name%> of self and other.
*/
static VALUE
<%=c_func(-1)%>(int argc, VALUE *argv, VALUE self)
{
    <% if !is_object %>
    VALUE klass, v;
    <% end %>
    if (argc < 1) {
        rb_raise(rb_eArgError,"wrong number of arguments (%d for >=1)",argc);
    }
    <% if is_object %>
    return <%=c_func%>_self(argc, argv, self);
    <% else %>
    klass = na_upcast(CLASS_OF(self),CLASS_OF(argv[0]));
    if (klass==cT) {
        return <%=c_func%>_self(argc, argv, self);
    } else {
        v = rb_funcall(klass, id_cast, 1, self);
        return rb_funcall2(v, rb_intern("<%=name%>"), argc, argv);
    }
    <% end %>
}
