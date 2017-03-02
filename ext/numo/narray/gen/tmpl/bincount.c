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
<%=c_func%>_<%=bits%>_wo_weight(VALUE self, size_t length)
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
<%=c_func%>_<%=fn%>_w_weight(VALUE self, VALUE weight, size_t length)
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
  <%=method.capitalize%> of self.
  @overload <%=method%>(*args)
  @param [Array of Numeric,Range] args  Affected dimensions.
  @return [Numo::<%=class_name%>] <%=method%> of self.
*/
static VALUE
<%=c_func%>(int argc, VALUE *argv, VALUE self)
{
    VALUE weight=Qnil, kw=Qnil;
    VALUE opts[1] = {Qundef};
    VALUE v, wclass;
    ID table[1];
    size_t length, minlength;

    table[0] = rb_intern("minlength");

    rb_scan_args(argc, argv, "01:", &weight, &kw);
    rb_get_kwargs(kw, table, 0, 1, opts);

  <% if is_unsigned %>
    v = numo_<%=type_name%>_max(0,0,self);
  <% else %>
    v = numo_<%=type_name%>_minmax(0,0,self);
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
            return <%=c_func%>_64_wo_weight(self, length);
        } else {
            return <%=c_func%>_32_wo_weight(self, length);
        }
    } else {
        wclass = CLASS_OF(weight);
        if (wclass == numo_cSFloat) {
            return <%=c_func%>_sf_w_weight(self, weight, length);
        } else {
            return <%=c_func%>_df_w_weight(self, weight, length);
        }
    }
}
