// ------- Integer count without weights -------
<%
[32,64].each do |bits|
   cnt_cT = "numo_cUInt#{bits}"
   cnt_type = "u_int#{bits}_t"
%>
static void
<%=c_iter%>_<%=bits%>(na_loop_t *const lp)
{
    size_t   i, x, n;
    char    *p1, *p2;
    ssize_t  s1, s2;
    size_t  *idx1;

    INIT_PTR_IDX(lp, 0, p1, s1, idx1);
    INIT_PTR(lp, 1, p2, s2);
    i = lp->args[0].shape[0];
    n = lp->args[1].shape[0];

    // initialize
    for (x=0; x < n; x++) {
        *(<%=cnt_type%>*)(p2 + s2*x) = 0;
    }

    if (idx1) {
        for (; i--;) {
            GET_DATA_INDEX(p1,idx1,dtype,x);
            (*(<%=cnt_type%>*)(p2 + s2*x))++;
        }
    } else {
        for (; i--;) {
            GET_DATA_STRIDE(p1,s1,dtype,x);
            (*(<%=cnt_type%>*)(p2 + s2*x))++;
        }
    }
}

static VALUE
<%=c_func%>_<%=bits%>(VALUE self, size_t length)
{
    size_t shape_out[1] = {length};
    ndfunc_arg_in_t ain[1] = {{cT,1}};
    ndfunc_arg_out_t aout[1] = {{<%=cnt_cT%>,1,shape_out}};
    ndfunc_t ndf = {<%=c_iter%>_<%=bits%>, NO_LOOP|NDF_STRIDE_LOOP|NDF_INDEX_LOOP,
                    1, 1, ain, aout};

    return na_ndloop(&ndf, 1, self);
}
<% end %>
// ------- end of Integer count without weights -------

// ------- Float count with weights -------
<%
[["SF","float"],
 ["DF","double"]].each do |fn,cnt_type|
  cnt_cT = "numo_c#{fn}loat"
  fn = fn.downcase
%>
static void
<%=c_iter%>_<%=fn%>(na_loop_t *const lp)
{
    <%=cnt_type%> w;
    size_t   i, x, n, m;
    char    *p1, *p2, *p3;
    ssize_t  s1, s2, s3;

    INIT_PTR(lp, 0, p1, s1);
    INIT_PTR(lp, 1, p2, s2);
    INIT_PTR(lp, 2, p3, s3);
    i = lp->args[0].shape[0];
    m = lp->args[1].shape[0];
    n = lp->args[2].shape[0];

    if (i != m) {
        rb_raise(nary_eShapeError,
                 "size mismatch along last axis between self and weight");
    }

    // initialize
    for (x=0; x < n; x++) {
        *(<%=cnt_type%>*)(p3 + s3*x) = 0;
    }
    for (; i--;) {
        GET_DATA_STRIDE(p1,s1,dtype,x);
        GET_DATA_STRIDE(p2,s2,<%=cnt_type%>,w);
        (*(<%=cnt_type%>*)(p3 + s3*x)) += w;
    }
}

static VALUE
<%=c_func%>_<%=fn%>(VALUE self, VALUE weight, size_t length)
{
    size_t shape_out[1] = {length};
    ndfunc_arg_in_t ain[2] = {{cT,1},{<%=cnt_cT%>,1}};
    ndfunc_arg_out_t aout[1] = {{<%=cnt_cT%>,1,shape_out}};
    ndfunc_t ndf = {<%=c_iter%>_<%=fn%>, NO_LOOP|NDF_STRIDE_LOOP,
                    2, 1, ain, aout};

    return na_ndloop(&ndf, 2, self, weight);
}
<% end %>
// ------- end of Float count with weights -------

/*
  Count the number of occurrences of each non-negative integer value.
  Only Integer-types has this method.

  @overload <%=name%>([weight], minlength:nil)
  @param [SFloat or DFloat or Array] weight (optional) Array of
    float values. Its size along last axis should be same as that of self.
  @param [Integer] minlength (keyword, optional) Minimum size along
    last axis for the output array.
  @return [UInt32 or UInt64 or SFloat or DFloat]
    Returns Float NArray if weight array is supplied,
    otherwise returns UInt32 or UInt64 depending on the size along last axis.
  @example
    Numo::Int32[0..4].bincount
    # => Numo::UInt32#shape=[5]
    # [1, 1, 1, 1, 1]

    Numo::Int32[0, 1, 1, 3, 2, 1, 7].bincount
    # => Numo::UInt32#shape=[8]
    # [1, 3, 1, 1, 0, 0, 0, 1]

    x = Numo::Int32[0, 1, 1, 3, 2, 1, 7, 23]
    x.bincount.size == x.max+1
    # => true

    w = Numo::DFloat[0.3, 0.5, 0.2, 0.7, 1.0, -0.6]
    x = Numo::Int32[0, 1, 1, 2, 2, 2]
    x.bincount(w)
    # => Numo::DFloat#shape=[3]
    # [0.3, 0.7, 1.1]

*/
static VALUE
<%=c_func(-1)%>(int argc, VALUE *argv, VALUE self)
{
    VALUE weight=Qnil, kw=Qnil;
    VALUE opts[1] = {Qundef};
    VALUE v, wclass;
    ID table[1] = {id_minlength};
    size_t length, minlength;

    rb_scan_args(argc, argv, "01:", &weight, &kw);
    rb_get_kwargs(kw, table, 0, 1, opts);

  <% if is_unsigned %>
    v = <%=type_name%>_max(0,0,self);
  <% else %>
    v = <%=type_name%>_minmax(0,0,self);
    if (m_num_to_data(RARRAY_AREF(v,0)) < 0) {
        rb_raise(rb_eArgError,"array items must be non-netagive");
    }
    v = RARRAY_AREF(v,1);
  <% end %>
    length = NUM2SIZET(v) + 1;

    if (opts[0] != Qundef) {
        minlength = NUM2SIZET(opts[0]);
        if (minlength > length) {
            length = minlength;
        }
    }

    if (NIL_P(weight)) {
        if (length > 4294967295ul) {
            return <%=c_func%>_64(self, length);
        } else {
            return <%=c_func%>_32(self, length);
        }
    } else {
        wclass = rb_obj_class(weight);
        if (wclass == numo_cSFloat) {
            return <%=c_func%>_sf(self, weight, length);
        } else {
            return <%=c_func%>_df(self, weight, length);
        }
    }
}
