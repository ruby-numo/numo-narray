/*
  Cast object to NArray::<%=class_name%>.
  @overload [](elements)
  @overload <%=op%>(array)
  @param [Numeric,Array] elements
  @param [Array] array
  @return [NArray::<%=class_name%>]
*/
static VALUE
<%=c_singleton_method%>(VALUE type, VALUE obj)
{
    VALUE v;
    narray_t *na;
    dtype x;

    if (CLASS_OF(obj)==cT) {
        return obj;
    }
    if (RTEST(rb_obj_is_kind_of(obj,rb_cNumeric))) {
        x = m_num_to_data(obj);
        return nary_<%=tp%>_new_dim0(x);
    }
    if (RTEST(rb_obj_is_kind_of(obj,rb_cArray))) {
        return <%=T["cast_array"].c_function%>(obj);
    }
    if (IsNArray(obj)) {
        GetNArray(obj,na);
        v = rb_narray_new(cT, NA_NDIM(na), NA_SHAPE(na));
        if (NA_SIZE(na)>0) {
            na_alloc_data(v);
            <%=T["store"].c_instance_method%>(v,obj);
        }
        return v;
    }
    rb_raise(nary_eCastError,"cannot cast to %s",rb_class2name(type));
    return Qnil;
}
