<% children.each do |c|%>
<%= c.result %>

<% end %>
/*
  Cast object to Numo::<%=class_name%>.
  @overload [](elements)
  @overload <%=name%>(array)
  @param [Numeric,Array] elements
  @param [Array] array
  @return [Numo::<%=class_name%>]
*/
static VALUE
<%=c_func(1)%>(VALUE type, VALUE obj)
{
    VALUE v;
    narray_t *na;
    dtype x;

    if (rb_obj_class(obj)==cT) {
        return obj;
    }
    if (RTEST(rb_obj_is_kind_of(obj,rb_cNumeric))) {
        x = m_num_to_data(obj);
        return <%=type_name%>_new_dim0(x);
    }
    if (RTEST(rb_obj_is_kind_of(obj,rb_cArray))) {
        return <%=find_tmpl("cast_array").c_func%>(obj);
    }
    if (IsNArray(obj)) {
        GetNArray(obj,na);
        v = nary_new(cT, NA_NDIM(na), NA_SHAPE(na));
        if (NA_SIZE(na) > 0) {
            <%=find_tmpl("store").c_func%>(v,obj);
        }
        return v;
    }
    if (rb_respond_to(obj,id_to_a)) {
        obj = rb_funcall(obj,id_to_a,0);
        if (TYPE(obj)!=T_ARRAY) {
            rb_raise(rb_eTypeError, "`to_a' did not return Array");
        }
        return <%=find_tmpl("cast_array").c_func%>(obj);
    }
    <% if is_object %>
    return robject_new_dim0(obj);
    <% else %>
    rb_raise(nary_eCastError,"cannot cast to %s",rb_class2name(type));
    return Qnil;
    <% end %>
}
