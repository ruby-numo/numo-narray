/*
  Cast object to Numo::<%=class_name%>.
  @overload [](elements)
  @overload <%=method%>(array)
  @param [Numeric,Array] elements
  @param [Array] array
  @return [Numo::<%=class_name%>]
*/
static VALUE
<%=c_func%>(VALUE type, VALUE obj)
{
    VALUE v;
    narray_t *na;
    dtype x;

    if (CLASS_OF(obj)==cT) {
        return obj;
    }
    if (RTEST(rb_obj_is_kind_of(obj,rb_cNumeric))) {
        x = m_num_to_data(obj);
        return numo_<%=tp%>_new_dim0(x);
    }
    if (RTEST(rb_obj_is_kind_of(obj,rb_cArray))) {
        return <%=find_tmpl("cast_array").c_func%>(obj);
    }
    if (IsNArray(obj)) {
        GetNArray(obj,na);
        v = rb_narray_new(cT, NA_NDIM(na), NA_SHAPE(na));
        if (NA_SIZE(na) > 0) {
            <%=find_tmpl("allocate").c_func%>(v);
            <%=find_tmpl("store").c_func%>(v,obj);
        }
        return v;
    }
    rb_raise(nary_eCastError,"cannot cast to %s",rb_class2name(type));
    return Qnil;
}
