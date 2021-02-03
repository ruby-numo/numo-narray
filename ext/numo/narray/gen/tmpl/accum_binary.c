//<% (is_float ? ["","_nan"] : [""]).each do |j| %>
static void
<%=c_iter%><%=j%>(na_loop_t *const lp)
{
    size_t   i, n;
    char    *p1, *p2, *p3;
    ssize_t  s1, s2, s3;

    INIT_COUNTER(lp, n);
    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR(lp, 2, p3, s3);

    if (s3==0) {
        dtype z;
        // Reduce loop
        GET_DATA(p3,dtype,z);
        for (i=0; i<n; i++) {
            dtype x, y;
            GET_DATA_STRIDE(p1,s1,dtype,x);
            GET_DATA_STRIDE(p2,s2,dtype,y);
            m_<%=name%><%=j%>(x,y,z);
        }
        SET_DATA(p3,dtype,z);
        return;
    } else {
        for (i=0; i<n; i++) {
            dtype x, y, z;
            GET_DATA_STRIDE(p1,s1,dtype,x);
            GET_DATA_STRIDE(p2,s2,dtype,y);
            GET_DATA(p3,dtype,z);
            m_<%=name%><%=j%>(x,y,z);
            SET_DATA_STRIDE(p3,s3,dtype,z);
        }
    }
}
//<% end %>

static VALUE
<%=c_func%>_self(int argc, VALUE *argv, VALUE self)
{
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
    //<% if is_float %>
    reduce = na_reduce_dimension(argc-1, argv+1, 2, naryv, &ndf, <%=c_iter%>_nan);
    //<% else %>
    reduce = na_reduce_dimension(argc-1, argv+1, 2, naryv, &ndf, 0);
    //<% end %>

    v =  na_ndloop(&ndf, 4, self, argv[0], reduce, m_<%=name%>_init);
    return <%=type_name%>_extract(v);
}

/*
  Binary <%=name%>.

<% if is_float %>
  @overload <%=op_map%>(other, axis:nil, keepdims:false, nan:false)
<% else %>
  @overload <%=op_map%>(other, axis:nil, keepdims:false)
<% end %>
  @param [Numo::NArray,Numeric] other
  @param [Numeric,Array,Range] axis  Performs <%=name%> along the axis.
  @param [TrueClass] keepdims (keyword) If true, the reduced axes are left in the result array as dimensions with size one.
<% if is_float %>
  @param [TrueClass] nan (keyword) If true, apply NaN-aware algorithm (avoid NaN if exists).
<% end %>
  @return [Numo::NArray] <%=name%> of self and other.
*/
static VALUE
<%=c_func(-1)%>(int argc, VALUE *argv, VALUE self)
{
    //<% if !is_object %>
    VALUE klass, v;
    //<% end %>
    if (argc < 1) {
        rb_raise(rb_eArgError,"wrong number of arguments (%d for >=1)",argc);
    }
    //<% if is_object %>
    return <%=c_func%>_self(argc, argv, self);
    //<% else %>
    klass = na_upcast(rb_obj_class(self),rb_obj_class(argv[0]));
    if (klass==cT) {
        return <%=c_func%>_self(argc, argv, self);
    } else {
        v = rb_funcall(klass, id_cast, 1, self);
        //<% if Gem::Version.create(RUBY_VERSION) < Gem::Version.create('2.7.0') %>
        return rb_funcall2(v, rb_intern("<%=name%>"), argc, argv);
        //<% else %>
        return rb_funcallv_kw(v, rb_intern("<%=name%>"), argc, argv, RB_PASS_CALLED_KEYWORDS);
        //<% end %>
    }
    //<% end %>
}
